/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * Background ctags-based symbol extraction.
 *
 * CtagsWorker runs inside a dedicated QThread and owns the ctags QProcess.
 * The worker thread and the GUI thread never share mutable data directly:
 * requests are delivered via queued signals and results are marshalled back
 * the same way. A generation counter makes sure stale results from superseded
 * requests are discarded, and a QMutex guards the worker's own request state -
 * this keeps the filter box and list refresh safe from data corruption while a
 * request is in flight.
 *
 * Requests are serialized: only one ctags process runs at a time. A request
 * for a file that is already being parsed restarts that parse; a request for
 * a different file is queued instead of killing the running process.
 *
 * Locating ctags is platform aware and never hard-codes a path: candidates are
 * collected from an environment override, the application directory (so a
 * portable/bundled build can ship its own copy), the well known installation
 * prefixes of the platform (Homebrew/MacPorts, /usr/local, /usr/bin, ...) and
 * finally every PATH entry. Each candidate is verified with a "--version" probe
 * before use, because the "ctags" name is also used by GNU Emacs etags and by
 * BSD ctags, which do not understand the command line used here. This is what
 * makes the lookup work on macOS/Linux, where several unrelated "ctags"
 * binaries can be installed side by side.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#ifndef CTAGSSYMBOLMANAGER_H
#define CTAGSSYMBOLMANAGER_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QMutex>
#include <QAtomicInt>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVector>

#include "FunctionListWidget.h"

class CtagsWorker : public QObject
{
    Q_OBJECT

public:
    explicit CtagsWorker(QObject *parent = nullptr);
    ~CtagsWorker() override;

public slots:
    // Executed in the worker thread. Starts a new parse, restarts the running
    // one when it is for the same file, or queues it when another file is
    // currently being parsed. Results are emitted via symbolsReady().
    void doRequest(int generation, const QString &filePath, const QString &ctagsLanguage);

    // Executed in the worker thread. Drops queued/running work for one file.
    void cancel(const QString &filePath);

    // Executed in the worker thread. Drops everything.
    void cancelAll();

    // Executed in the worker thread right after start up. Fills the language and
    // file name pattern caches so that the GUI thread can answer canParse()
    // without ever waiting for a subprocess, then emits knowledgeReady().
    void warmUp();

signals:
    void symbolsReady(int generation, const QString &filePath, const QVector<FunctionSymbol> &symbols);

    // Emitted at the end of warmUp(); forwarded queued to the GUI thread.
    void knowledgeReady();

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    struct PendingRequest
    {
        int generation = 0;
        QString filePath;
        QString ctagsLanguage;
    };

    QVector<FunctionSymbol> parseOutput(const QByteArray &output) const;
    QString usableCtagsExecutable();
    void startProcess(int generation, const QString &filePath, const QString &ctagsLanguage);
    void abortProcessLocked(); // caller must hold mutex
    void startNextPending();

    QMutex mutex; // guards process/outputBuffer/current*/pending below
    QProcess *process = nullptr;
    QByteArray outputBuffer;
    QByteArray errorBuffer;
    QString currentFile;
    QString currentLanguage;
    QString currentExecutable;
    int currentGen = 0;
    int startRetries = 0; // bounded re-tries after a binary failed to launch
    int forceRetries = 0; // bounded re-tries without --language-force
    QVector<PendingRequest> pending;
};

class CtagsSymbolManager : public QObject
{
    Q_OBJECT

public:
    explicit CtagsSymbolManager(QObject *parent = nullptr);
    ~CtagsSymbolManager() override;

    // Returns the generation id of this request. A newer request for the same
    // file supersedes this one, so only its result reaches symbolsReady().
    int request(const QString &filePath, const QString &ctagsLanguage);

    // Invalidates the pending result for one file (used when a file is closed)
    void cancel(const QString &filePath);

    // Invalidates every pending result
    void cancelAll();

    // True while a request for this file has been issued but not yet delivered
    bool isPending(const QString &filePath) const;

    // Path of the ctags binary that should be tried next, or an empty string
    // when nothing usable is left. Cheap and non-blocking (candidate existence
    // checks only), so it is safe to call from the GUI thread; the actual
    // "--version" verification happens in validateCtags() on the worker thread.
    static QString ctagsExecutable();

    // Every language the located ctags knows, as reported by
    // "ctags --list-languages". Cached after the first successful call.
    //
    // This is what lets the editor translate its own language names without a
    // hand written table: a name that normalizes to the same string as a ctags
    // language is passed as "--language-force", which corrects file extensions
    // ctags would otherwise misread (".v" is taken for V, not Verilog). A name
    // without a match is left to ctags' own file name detection instead, so an
    // incomplete mapping degrades gracefully rather than failing.
    //
    // The first call starts a subprocess (about 30 ms) - warm it up from a
    // background thread. Returns an empty list when there is no usable ctags.
    static QStringList knownLanguages();

    // Everything the located ctags claims to be able to parse, taken from
    // "ctags --list-maps": lower cased file extensions ("c", "py") and lower
    // cased whole file names ("makefile", "cmakelists.txt"). Cached exactly like
    // knownLanguages(); returns an empty set when there is no usable ctags.
    static QSet<QString> knownFilePatterns();

    // False while the two probes above have not finished yet (they run on the
    // worker thread at start up). Callers that have to decide something in the
    // meantime should assume "yes" and re-evaluate once knowledgeReady() fires.
    static bool knowledgeLoaded();

    // True when ctags is able to produce symbols for this file: either the
    // editor's language name resolved onto a ctags language (ctagsLanguage, see
    // MainWindow::ctagsLanguageFor()) or ctags itself claims the file's name or
    // extension. Purely in-memory lookups, so the GUI thread can call this for
    // every file without blocking.
    static bool canParse(const QString &filePath, const QString &ctagsLanguage);

    // True once a parse result - including an empty one - has been delivered for
    // this file, so callers can tell "not parsed yet" from "nothing to find".
    bool hasParsed(const QString &filePath) const;

    // Forgets everything about a file (pending request and delivered result) so
    // that the next update really re-runs ctags. Used when a file is saved,
    // reloaded or renamed, and when its panel is switched off.
    void invalidate(const QString &filePath);

    // Verifies and memoizes that the given binary really is Universal Ctags or
    // Exuberant Ctags (the two implementations supporting the command line used
    // here). Starts a subprocess, so only call it from a background thread.
    static bool validateCtags(const QString &executable);

    // Marks a candidate as unusable so that lookups skip it from now on.
    static void rejectCtags(const QString &executable);

signals:
    void symbolsReady(int generation, const QString &filePath, const QVector<FunctionSymbol> &symbols);

    // Forwarded queued from the worker thread once the ctags language and file
    // name pattern caches are populated. Consumers re-evaluate the open files
    // then, so a decision that had to be guessed before the probes finished is
    // corrected.
    void knowledgeReady();

private slots:
    void onSymbolsReady(int generation, const QString &filePath, const QVector<FunctionSymbol> &symbols);

private:
    QThread workerThread;
    CtagsWorker *worker = nullptr;
    QAtomicInt generationCounter{0};

    // GUI thread only: newest generation per file, entry erased on delivery
    QHash<QString, int> pendingByFile;

    // GUI thread only: files whose result was already delivered
    QSet<QString> parsedFiles;
};

#endif // CTAGSSYMBOLMANAGER_H
