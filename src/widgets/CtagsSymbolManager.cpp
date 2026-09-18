/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * Background ctags-based symbol extraction - implementation.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#include "CtagsSymbolManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {
// ctags writes to stdout ('-f -'), so no temporary files are ever created
// on disk. Every symbol line looks like:
//   name<TAB>file<TAB>/^pattern$/;"<TAB>kind<TAB>line:N[<TAB>extra...]
// The extension fields (kind, line:N, ...) follow the tab-separated field that
// carries the /^pattern$/;" part - the ;" marker is appended to the pattern
// itself, it is NOT a field of its own.
constexpr int MaxOutputBytes = 32 * 1024 * 1024; // safety cap: 32 MB

// Upper bound of candidates probed for a single parse. Only used as a guard
// against pathological PATH entries.
constexpr int MaxCtagsCandidates = 32;

#ifdef Q_OS_WIN
constexpr const char *CtagsBinaryName = "ctags.exe";
#else
constexpr const char *CtagsBinaryName = "ctags";
#endif

// The candidate cache below is shared between the GUI thread (which calls
// ctagsExecutable() to decide whether to bother requesting a parse at all) and
// the worker thread (which validates and runs the binary).
QMutex ctagsMutex;
QStringList cachedCandidates;
bool candidatesCached = false;
QSet<QString> rejectedCtags;        // proven unusable, skipped from now on
QHash<QString, bool> ctagsValidity; // memoized "--version" probe results
bool warnedNoCtags = false;
QHash<QString, QStringList> knownLanguagesCache; // executable -> --list-languages
QHash<QString, QSet<QString>> knownFilePatternsCache; // executable -> --list-maps

int extensionFieldIndex(const QStringList &fields)
{
    // Standard case: the marker is glued to the pattern in field 2
    if (fields.size() > 3 && fields.at(2).endsWith(QStringLiteral(";\"")))
        return 3;

    // Defensive: some ctags builds emit the marker as its own field
    for (int i = 2; i < fields.size(); ++i) {
        if (fields.at(i) == QStringLiteral(";\""))
            return i + 1;
    }

    return -1;
}

// ctags echoes identifier bytes exactly as they appear in the source file, so a
// GBK/Shift-JIS file on Windows produces bytes that are not valid UTF-8. Fall
// back to the local 8-bit codec in that case (which is UTF-8 on macOS/Linux and
// the ANSI code page on Windows) so symbol names are shown correctly instead of
// as replacement characters.
QString decodeTagLine(const QByteArray &raw)
{
    const QString text = QString::fromUtf8(raw);
    if (text.contains(QChar::ReplacementCharacter))
        return QString::fromLocal8Bit(raw);
    return text;
}

// Normalizes an existing file into a canonical path so that symlinked or
// differently spelled candidates are probed only once. Returns an empty string
// when the candidate does not exist (directories are not candidates).
QString normalizeCandidate(const QString &path)
{
    const QFileInfo info(path);
    if (!info.isFile())
        return QString();
    const QString canonical = info.canonicalFilePath();
    return canonical.isEmpty() ? info.absoluteFilePath() : canonical;
}

void appendCandidate(QStringList &out, QSet<QString> &seen, const QString &path)
{
    const QString normalized = normalizeCandidate(path);
    if (normalized.isEmpty() || seen.contains(normalized))
        return;
    seen.insert(normalized);
    out.append(normalized);
}

// Every PATH entry, in order. Some toolchains (for example the MinGW bundles
// shipped with GCC distributions) install GNU Emacs etags under the name
// "ctags"; such an entry has to be rejectable so that a later PATH entry or a
// well known location can still be used. That is why PATH is enumerated
// instead of stopping at the first hit like QStandardPaths::findExecutable does.
QStringList pathDirectories()
{
    QStringList directories;
    const QString rawPath = QString::fromLocal8Bit(qgetenv("PATH"));
    const QStringList parts = rawPath.split(QDir::listSeparator(), Qt::SkipEmptyParts);

    for (QString part : parts) {
        part.remove(QLatin1Char('"'));
        part = part.trimmed();
        if (!part.isEmpty())
            directories.append(part);
    }

    return directories;
}

// Ordered list of locations worth trying, on every platform.
QStringList collectCandidates()
{
    QStringList out;
    QSet<QString> seen;

    // 1. Explicit override - escape hatch for unusual installations.
    const QByteArray override = qgetenv("NOTEPADNEXT_CTAGS");
    if (!override.isEmpty())
        appendCandidate(out, seen, QFile::decodeName(override));

    // 2. Next to the executable, so a portable or bundled build can carry its
    //    own copy (macOS bundle: Contents/MacOS, then Contents/Resources).
    const QString appDir = QCoreApplication::applicationDirPath();
    appendCandidate(out, seen, appDir + QLatin1Char('/') + QLatin1String(CtagsBinaryName));
#ifdef Q_OS_MACOS
    appendCandidate(out, seen,
                    appDir + QLatin1String("/../Resources/") + QLatin1String(CtagsBinaryName));
#endif

    // 3. Well known installation prefixes. GUI applications on macOS inherit
    //    only a minimal PATH from launchd, so a Homebrew/MacPorts install has to
    //    be listed explicitly or it would never be found.
#ifdef Q_OS_MACOS
    appendCandidate(out, seen, QStringLiteral("/opt/homebrew/bin/ctags")); // Homebrew, Apple Silicon
    appendCandidate(out, seen, QStringLiteral("/usr/local/bin/ctags"));    // Homebrew, Intel
    appendCandidate(out, seen, QStringLiteral("/opt/local/bin/ctags"));    // MacPorts
    appendCandidate(out, seen, QStringLiteral("/usr/bin/ctags"));          // ships with Xcode tools
#elif !defined(Q_OS_WIN)
    appendCandidate(out, seen, QStringLiteral("/usr/local/bin/ctags"));
    appendCandidate(out, seen, QStringLiteral("/usr/bin/ctags"));
    appendCandidate(out, seen, QStringLiteral("/snap/bin/ctags"));
    appendCandidate(out, seen, QStringLiteral("/var/lib/flatpak/exports/bin/ctags"));
#endif

    appendCandidate(out, seen,
                    QDir::homePath() + QLatin1String("/.local/bin/") + QLatin1String(CtagsBinaryName));

    // 4. Plain PATH lookup, every entry in order.
    const QStringList directories = pathDirectories();
    for (const QString &directory : directories)
        appendCandidate(out, seen, QDir(directory).filePath(QLatin1String(CtagsBinaryName)));

    return out;
}

QStringList ctagsCandidates()
{
    QMutexLocker locker(&ctagsMutex);
    if (!candidatesCached) {
        cachedCandidates = collectCandidates();
        candidatesCached = true;
    }
    return cachedCandidates;
}

// A binary is only accepted when it identifies itself as Universal Ctags or
// Exuberant Ctags: those are the implementations supporting the command line
// used here. GNU Emacs etags ("etags (GNU Emacs ...)") and BSD ctags print a
// different banner and are rejected.
bool probeCtags(const QString &executable)
{
    QProcess probe;
    probe.setProgram(executable);
    probe.setArguments(QStringList() << QStringLiteral("--version"));
    probe.start();

    if (!probe.waitForStarted(3000))
        return false;

    if (!probe.waitForFinished(5000)) {
        probe.kill();
        probe.waitForFinished(1000);
        return false;
    }

    const QByteArray banner = probe.readAllStandardOutput() + probe.readAllStandardError();
    const QString text = QString::fromLatin1(banner);

    return text.contains(QLatin1String("Universal Ctags"), Qt::CaseInsensitive)
        || text.contains(QLatin1String("Exuberant Ctags"), Qt::CaseInsensitive);
}
} // namespace

CtagsWorker::CtagsWorker(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<FunctionSymbol>();
    qRegisterMetaType<QVector<FunctionSymbol>>();
}

CtagsWorker::~CtagsWorker()
{
    QMutexLocker locker(&mutex);
    pending.clear();
    abortProcessLocked();
}

void CtagsWorker::doRequest(int generation, const QString &filePath, const QString &ctagsLanguage)
{
    QMutexLocker locker(&mutex);

    // A newer request for the same file replaces anything still queued
    for (int i = pending.size() - 1; i >= 0; --i) {
        if (pending.at(i).filePath == filePath)
            pending.removeAt(i);
    }

    if (process != nullptr && currentFile == filePath) {
        // Same file requested again (e.g. it was saved): restart from scratch
        abortProcessLocked();
    }
    else if (process != nullptr) {
        // Another file is being parsed right now - never kill it, queue instead
        pending.append({generation, filePath, ctagsLanguage});
        return;
    }

    startRetries = 0;
    forceRetries = 0;
    locker.unlock();
    startProcess(generation, filePath, ctagsLanguage);
}

void CtagsWorker::cancel(const QString &filePath)
{
    QMutexLocker locker(&mutex);

    for (int i = pending.size() - 1; i >= 0; --i) {
        if (pending.at(i).filePath == filePath)
            pending.removeAt(i);
    }

    if (currentFile == filePath)
        abortProcessLocked();
}

void CtagsWorker::cancelAll()
{
    QMutexLocker locker(&mutex);
    pending.clear();
    abortProcessLocked();
    currentFile.clear();
}

// Runs on the worker thread at start up. Both probes spawn ctags once and cache
// the result for the whole process, which is what keeps canParse() free of
// subprocesses on the GUI thread later on.
void CtagsWorker::warmUp()
{
    CtagsSymbolManager::knownLanguages();
    CtagsSymbolManager::knownFilePatterns();
    emit knowledgeReady();
}

// Walks the candidate list until one binary passes verification. Candidates
// that turn out not to be a usable ctags build are rejected permanently, so
// every later parse starts one step further down the list.
QString CtagsWorker::usableCtagsExecutable()
{
    for (int attempt = 0; attempt < MaxCtagsCandidates; ++attempt) {
        const QString candidate = CtagsSymbolManager::ctagsExecutable();
        if (candidate.isEmpty())
            return QString();

        if (CtagsSymbolManager::validateCtags(candidate))
            return candidate;

        CtagsSymbolManager::rejectCtags(candidate);
    }

    return QString();
}

void CtagsWorker::startProcess(int generation, const QString &filePath, const QString &ctagsLanguage)
{
    if (!QFileInfo::exists(filePath)) {
        // Deleted, or an unsaved buffer: nothing to parse, but the caller must
        // still get an answer so the pending flag is cleared.
        emit symbolsReady(generation, filePath, {});
        startNextPending();
        return;
    }

    const QString ctagsPath = usableCtagsExecutable();
    if (ctagsPath.isEmpty()) {
        emit symbolsReady(generation, filePath, {});
        startNextPending();
        return;
    }

    QStringList args;
    args << QStringLiteral("-f") << QStringLiteral("-")      // tags to stdout
         << QStringLiteral("--sort=no")                       // keep file order
         << QStringLiteral("--fields=+n");                    // add line numbers

    // The language is forced instead of relying on the file name so that header
    // files are parsed in the language of their editor. Older builds do not know
    // every language, in which case the parse is retried without this option
    // (see onProcessFinished): that keeps .sv/.svh and friends working on
    // Exuberant Ctags.
    if (!ctagsLanguage.isEmpty())
        args << (QStringLiteral("--language-force=") + ctagsLanguage);

    args << QDir::toNativeSeparators(filePath);               // identity on POSIX

    {
        QMutexLocker locker(&mutex);
        currentGen = generation;
        currentFile = filePath;
        currentLanguage = ctagsLanguage;
        currentExecutable = ctagsPath;
        outputBuffer.clear();
        errorBuffer.clear();

        process = new QProcess(this);
        process->setWorkingDirectory(QFileInfo(filePath).absolutePath());

        connect(process, &QProcess::readyReadStandardOutput,
                this, &CtagsWorker::onReadyReadStandardOutput);
        connect(process, &QProcess::readyReadStandardError,
                this, &CtagsWorker::onReadyReadStandardError);
        connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
                this, &CtagsWorker::onProcessFinished);
        connect(process, &QProcess::errorOccurred,
                this, &CtagsWorker::onProcessError);
    }

    // Started outside the lock: QProcess::start() may emit signals synchronously
    process->start(ctagsPath, args);
}

void CtagsWorker::abortProcessLocked()
{
    if (process == nullptr)
        return;

    // Disconnect first: onProcessFinished() must not run re-entrantly while
    // the mutex is held (that is what used to deadlock). The killed process is
    // simply discarded instead - no waitForFinished() is needed.
    process->disconnect(this);
    if (process->state() != QProcess::NotRunning)
        process->kill();
    process->deleteLater();
    process = nullptr;
    outputBuffer.clear();
    errorBuffer.clear();
}

void CtagsWorker::startNextPending()
{
    PendingRequest next;
    {
        QMutexLocker locker(&mutex);
        if (process != nullptr || pending.isEmpty())
            return;
        next = pending.takeFirst();
        startRetries = 0;
        forceRetries = 0;
    }
    startProcess(next.generation, next.filePath, next.ctagsLanguage);
}

void CtagsWorker::onReadyReadStandardOutput()
{
    QMutexLocker locker(&mutex);
    if (!process)
        return;

    outputBuffer += process->readAllStandardOutput();

    if (outputBuffer.size() > MaxOutputBytes) {
        // Absurdly large output - drop this parse instead of exhausting memory.
        // An (empty) result is still delivered so that the pending flag is
        // cleared and the queue keeps moving.
        const int gen = currentGen;
        const QString file = currentFile;
        abortProcessLocked();
        locker.unlock();

        if (gen != 0)
            emit symbolsReady(gen, file, {});
        startNextPending();
    }
}

void CtagsWorker::onReadyReadStandardError()
{
    QMutexLocker locker(&mutex);
    if (!process)
        return;
    // Only kept to classify failures (unknown language, ...); ctags writes all
    // tag data to stdout.
    errorBuffer += process->readAllStandardError();
    if (errorBuffer.size() > 8192)
        errorBuffer = errorBuffer.right(8192);
}

void CtagsWorker::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QByteArray output;
    QByteArray errors;
    int gen = 0;
    QString file;
    QString language;

    {
        QMutexLocker locker(&mutex);
        if (!process)
            return;

        output = outputBuffer;
        errors = errorBuffer;
        gen = currentGen;
        file = currentFile;
        language = currentLanguage;

        outputBuffer.clear();
        errorBuffer.clear();
        currentFile.clear();
        currentLanguage.clear();
        currentExecutable.clear();
        process->deleteLater();
        process = nullptr;
    }

    if (exitStatus != QProcess::NormalExit || gen == 0) {
        // The parser died (crash, killed, ...). An empty result still has to be
        // reported, otherwise the pending flag for this file would never clear.
        startRetries = 0;
        forceRetries = 0;
        if (gen != 0 && exitStatus != QProcess::NormalExit)
            emit symbolsReady(gen, file, {});
        startNextPending();
        return;
    }

    // ctags rejects a language it does not know with "Unknown language ..." and
    // exit code 1 (Exuberant Ctags, still shipped by macOS, lacks SystemVerilog
    // for instance). Retry once letting ctags pick the language from the file
    // name instead of giving up with an empty list.
    if (exitCode != 0 && !language.isEmpty()
            && QString::fromLatin1(errors).contains(QStringLiteral("language"), Qt::CaseInsensitive)
            && forceRetries < 1) {
        ++forceRetries;
        ++startRetries; // keep the re-tries bounded as a whole
        startProcess(gen, file, QString());
        return;
    }

    startRetries = 0;
    forceRetries = 0;

    // The manager decides per file whether a result is still wanted
    emit symbolsReady(gen, file, parseOutput(output));

    startNextPending();
}

// QProcess does not emit finished() when a program could not be launched at all
// (wrong architecture, missing shared library, no execute permission, ...).
// Without handling this the worker would wait forever and every later request
// would pile up in the queue. The offending binary is blacklisted and the parse
// is retried with the next candidate.
void CtagsWorker::onProcessError(QProcess::ProcessError error)
{
    if (error != QProcess::FailedToStart)
        return; // the remaining error kinds are followed by finished()

    int gen = 0;
    QString file;
    QString language;
    QString executable;

    {
        QMutexLocker locker(&mutex);
        if (!process)
            return;

        gen = currentGen;
        file = currentFile;
        language = currentLanguage;
        executable = currentExecutable;

        process->disconnect(this);
        process->deleteLater();
        process = nullptr;
        outputBuffer.clear();
        errorBuffer.clear();
        currentFile.clear();
        currentLanguage.clear();
        currentExecutable.clear();
    }

    CtagsSymbolManager::rejectCtags(executable);

    if (!file.isEmpty() && startRetries < 3) {
        ++startRetries;
        startProcess(gen, file, language);
        return;
    }

    startRetries = 0;
    forceRetries = 0;

    if (gen != 0)
        emit symbolsReady(gen, file, {});
    startNextPending();
}

QVector<FunctionSymbol> CtagsWorker::parseOutput(const QByteArray &output) const
{
    QVector<FunctionSymbol> result;
    const QList<QByteArray> lines = output.split('\n');

    for (const QByteArray &rawLine : lines) {
        const QString line = decodeTagLine(rawLine).trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('!')))
            continue; // header/comment lines

        const QStringList fields = line.split(QLatin1Char('\t'));
        if (fields.size() < 4)
            continue;

        const int extIndex = extensionFieldIndex(fields);
        if (extIndex < 0)
            continue;

        FunctionSymbol sym;
        sym.name = fields.at(0);
        if (sym.name.isEmpty())
            continue;

        // Everything after the ;" marker: kind letter, line:N, file:, scope:...
        bool kindSeen = false;
        for (int i = extIndex; i < fields.size(); ++i) {
            const QString &f = fields.at(i);
            if (f.startsWith(QStringLiteral("line:"))) {
                sym.line = f.mid(5).toInt();
            }
            else if (f.startsWith(QStringLiteral("kind:"))) {
                sym.kind = f.mid(5);
                kindSeen = true;
            }
            else if (!kindSeen && !f.contains(QLatin1Char(':'))) {
                // Classic single letter kind: f, p, m, v, d, s, t, c, ...
                sym.kind = f;
                kindSeen = true;
            }
        }

        if (sym.line > 0)
            result.append(sym);
    }

    return result;
}


CtagsSymbolManager::CtagsSymbolManager(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<FunctionSymbol>();
    qRegisterMetaType<QVector<FunctionSymbol>>();

    worker = new CtagsWorker();
    worker->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    // Queued back to the GUI thread
    connect(worker, &CtagsWorker::symbolsReady, this, &CtagsSymbolManager::onSymbolsReady, Qt::QueuedConnection);
    connect(worker, &CtagsWorker::knowledgeReady, this, &CtagsSymbolManager::knowledgeReady, Qt::QueuedConnection);

    workerThread.start();

    // Resolving the language list and the file name maps runs a subprocess each,
    // so warm them up on the worker thread right away. By the time the first file
    // is opened the caches are populated and the GUI thread never has to wait for
    // a probe. knowledgeReady() is emitted when they are done, which is after any
    // connection made by the code that constructs this manager, so it can never
    // be missed.
    QMetaObject::invokeMethod(worker, "warmUp", Qt::QueuedConnection);
}

CtagsSymbolManager::~CtagsSymbolManager()
{
    cancelAll();
    workerThread.quit();
    workerThread.wait(5000);
}

int CtagsSymbolManager::request(const QString &filePath, const QString &ctagsLanguage)
{
    const int gen = generationCounter.fetchAndAddRelaxed(1) + 1;
    pendingByFile.insert(filePath, gen);

    // Queued to the worker thread; requests are processed in order
    QMetaObject::invokeMethod(worker, "doRequest", Qt::QueuedConnection,
                              Q_ARG(int, gen),
                              Q_ARG(QString, filePath),
                              Q_ARG(QString, ctagsLanguage));
    return gen;
}

void CtagsSymbolManager::cancel(const QString &filePath)
{
    invalidate(filePath);
    QMetaObject::invokeMethod(worker, "cancel", Qt::QueuedConnection,
                              Q_ARG(QString, filePath));
}

void CtagsSymbolManager::cancelAll()
{
    pendingByFile.clear();
    parsedFiles.clear();
    QMetaObject::invokeMethod(worker, "cancelAll", Qt::QueuedConnection);
}

void CtagsSymbolManager::invalidate(const QString &filePath)
{
    // Dropping the pending generation is enough for an in-flight result to be
    // discarded when it arrives, so the worker is left alone here: the request
    // that normally follows right away wants the file parsed anyway.
    pendingByFile.remove(filePath);
    parsedFiles.remove(filePath);
}

bool CtagsSymbolManager::isPending(const QString &filePath) const
{
    return pendingByFile.contains(filePath);
}

bool CtagsSymbolManager::hasParsed(const QString &filePath) const
{
    return parsedFiles.contains(filePath);
}

void CtagsSymbolManager::onSymbolsReady(int generation, const QString &filePath, const QVector<FunctionSymbol> &symbols)
{
    // Drop results superseded by a newer request for the same file
    auto it = pendingByFile.find(filePath);
    if (it == pendingByFile.end() || it.value() != generation)
        return;
    pendingByFile.erase(it);

    // An empty result is a result: it marks the file as "done, nothing to find"
    // so that switching tabs does not re-run ctags for it over and over.
    parsedFiles.insert(filePath);

    // Re-emit on the GUI thread; consumers validate the file path
    emit symbolsReady(generation, filePath, symbols);
}

QString CtagsSymbolManager::ctagsExecutable()
{
    const QStringList candidates = ctagsCandidates();

    QMutexLocker locker(&ctagsMutex);
    for (const QString &candidate : candidates) {
        if (!rejectedCtags.contains(candidate))
            return candidate;
    }

    if (!warnedNoCtags) {
        warnedNoCtags = true;
        if (candidates.isEmpty()) {
            qWarning("FunctionList: no ctags executable found, the function list will stay empty. "
                     "Install Universal Ctags (macOS: brew install universal-ctags / Linux: "
                     "apt install universal-ctags / Windows: https://ctags.io) or set the "
                     "NOTEPADNEXT_CTAGS environment variable to its full path.");
        }
        else {
            qWarning("FunctionList: none of the %d ctags candidates is usable, the function list "
                     "will stay empty", int(candidates.size()));
        }
    }

    return QString();
}

QStringList CtagsSymbolManager::knownLanguages()
{
    const QString executable = ctagsExecutable();
    if (executable.isEmpty())
        return QStringList();

    {
        QMutexLocker locker(&ctagsMutex);
        const auto it = knownLanguagesCache.constFind(executable);
        if (it != knownLanguagesCache.constEnd())
            return it.value();
    }

    QStringList languages;

    // Outside the lock: the probe takes ~30 ms and nothing here depends on
    // holding it (a concurrent duplicate probe would only repeat harmless work).
    QProcess probe;
    probe.setProgram(executable);
    probe.setArguments(QStringList() << QStringLiteral("--list-languages"));
    probe.start();

    if (probe.waitForStarted(3000) && probe.waitForFinished(10000)) {
        const QStringList lines = QString::fromUtf8(probe.readAllStandardOutput())
                                      .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for (const QString &rawLine : lines) {
            const QString line = rawLine.trimmed();
            // Placeholders such as "Unknown [disabled]" cannot be forced.
            if (line.isEmpty() || line.contains(QLatin1Char('[')))
                continue;
            languages.append(line);
        }
    }
    else {
        probe.kill();
        probe.waitForFinished(1000);
    }

    if (languages.isEmpty()) {
        qWarning("FunctionList: 'ctags --list-languages' produced no usable output; "
                 "language names will be left to ctags' own file name detection");
    }

    QMutexLocker locker(&ctagsMutex);
    knownLanguagesCache.insert(executable, languages);
    return languages;
}

// Expands the "[Mm]" style character classes of a ctags file name pattern, e.g.
// "[Mm]akefile" -> { "Makefile", "makefile" }. Returns an empty list when the
// pattern is malformed or needs more combinations than a name is worth.
QStringList expandNamePattern(const QString &pattern, int limit = 8)
{
    QStringList alternatives;
    alternatives.append(QString());

    for (int i = 0; i < pattern.size(); ++i) {
        const QChar character = pattern.at(i);

        if (character != QLatin1Char('[')) {
            for (QString &alternative : alternatives)
                alternative.append(character);
            continue;
        }

        const int closing = pattern.indexOf(QLatin1Char(']'), i + 1);
        if (closing < 0)
            return QStringList(); // malformed pattern, ignore it

        QStringList expanded;
        const QString members = pattern.mid(i + 1, closing - i - 1);
        for (const QString &alternative : alternatives) {
            for (const QChar member : members) {
                expanded.append(alternative + member);
                if (expanded.size() > limit)
                    return QStringList();
            }
        }

        alternatives = expanded;
        i = closing;
    }

    return alternatives;
}

QSet<QString> CtagsSymbolManager::knownFilePatterns()
{
    const QString executable = ctagsExecutable();
    if (executable.isEmpty())
        return QSet<QString>();

    {
        QMutexLocker locker(&ctagsMutex);
        const auto it = knownFilePatternsCache.constFind(executable);
        if (it != knownFilePatternsCache.constEnd())
            return it.value();
    }

    QSet<QString> patterns;

    // "ctags --list-maps" prints one line per language:
    //   C        *.c *.h
    //   Asm      *.A51 *.[68][68][kKsSxX] *.asm ...
    //   Make     Makefile makefile *.mk
    // The columns are separated by runs of blanks (space padded, NOT tab
    // separated, which is why simplified() is used before splitting) and the
    // lines may end in CRLF. Only literal patterns are kept: a real glob such as
    // *.[68][68]k can never match a plain extension test anyway.
    QProcess probe;
    probe.setProgram(executable);
    probe.setArguments(QStringList() << QStringLiteral("--list-maps"));
    probe.start();

    if (probe.waitForStarted(3000) && probe.waitForFinished(10000)) {
        const QStringList lines = QString::fromUtf8(probe.readAllStandardOutput())
                                      .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
        for (const QString &rawLine : lines) {
            const QStringList columns = rawLine.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
            // Column 0 is the language name, the rest are file name patterns
            for (int i = 1; i < columns.size(); ++i) {
                const QString pattern = columns.at(i);
                if (pattern.isEmpty())
                    continue;

                if (pattern.startsWith(QStringLiteral("*."))) {
                    const QString extension = pattern.mid(2);
                    if (extension.contains(QLatin1Char('*')) || extension.contains(QLatin1Char('?'))
                            || extension.contains(QLatin1Char('[')) || extension.contains(QLatin1Char(']')))
                        continue; // a glob, not a plain extension
                    if (!extension.isEmpty())
                        patterns.insert(extension.toLower());
                }
                else if (!pattern.contains(QLatin1Char('*')) && !pattern.contains(QLatin1Char('?'))) {
                    // A whole file name such as "Makefile" or "CMakeLists.txt".
                    // Character classes ("[Mm]akefile", "[._]vimrc") are expanded
                    // into their alternatives.
                    const QStringList names = pattern.contains(QLatin1Char('['))
                            ? expandNamePattern(pattern)
                            : QStringList(pattern);
                    for (const QString &name : names) {
                        if (!name.isEmpty())
                            patterns.insert(name.toLower());
                    }
                }
            }
        }
    }
    else {
        probe.kill();
        probe.waitForFinished(1000);
    }

    if (patterns.isEmpty()) {
        qWarning("FunctionList: 'ctags --list-maps' produced no usable output; "
                 "file names will only be recognised through their language name");
    }

    QMutexLocker locker(&ctagsMutex);
    knownFilePatternsCache.insert(executable, patterns);
    return patterns;
}

bool CtagsSymbolManager::knowledgeLoaded()
{
    const QString executable = ctagsExecutable();
    if (executable.isEmpty())
        return true; // "there is no ctags" is knowledge as well

    QMutexLocker locker(&ctagsMutex);
    return knownLanguagesCache.contains(executable) && knownFilePatternsCache.contains(executable);
}

bool CtagsSymbolManager::canParse(const QString &filePath, const QString &ctagsLanguage)
{
    if (ctagsExecutable().isEmpty())
        return false; // no ctags: there is nothing the panel could ever show

    if (!ctagsLanguage.isEmpty())
        return true; // the editor's language resolved onto a ctags language

    // The probes spawn a subprocess, so they run in the background and may still
    // be pending right after start up. Assume "yes" in that case: the caller
    // re-evaluates once knowledgeReady() arrives, and a panel that only appears
    // a moment later beats one that never appears at all.
    if (!knowledgeLoaded())
        return true;

    const QFileInfo info(filePath);
    const QSet<QString> patterns = knownFilePatterns();

    // QFileInfo::suffix() is empty for names like "Makefile", which is why whole
    // file names are part of the pattern set as well.
    const QString suffix = info.suffix().toLower();
    if (!suffix.isEmpty() && patterns.contains(suffix))
        return true;

    return patterns.contains(info.fileName().toLower());
}

bool CtagsSymbolManager::validateCtags(const QString &executable)
{
    if (executable.isEmpty())
        return false;

    {
        QMutexLocker locker(&ctagsMutex);
        const auto it = ctagsValidity.constFind(executable);
        if (it != ctagsValidity.constEnd())
            return it.value();
    }

    // Runs on the worker thread: the probe never blocks the GUI
    const bool valid = probeCtags(executable);

    QMutexLocker locker(&ctagsMutex);
    ctagsValidity.insert(executable, valid);
    if (valid)
        qInfo("FunctionList: using ctags at '%s'", qUtf8Printable(executable));
    return valid;
}

void CtagsSymbolManager::rejectCtags(const QString &executable)
{
    if (executable.isEmpty())
        return;

    QMutexLocker locker(&ctagsMutex);

    if (!rejectedCtags.contains(executable))
        qWarning("FunctionList: ignoring '%s' - it is not a usable ctags build, trying the next candidate",
                 qUtf8Printable(executable));

    rejectedCtags.insert(executable);
    ctagsValidity.insert(executable, false);
    candidatesCached = false; // the candidate list may contain duplicates of it
    warnedNoCtags = false;
    knownLanguagesCache.remove(executable); // its language list is not trustworthy
    knownFilePatternsCache.remove(executable);
}
