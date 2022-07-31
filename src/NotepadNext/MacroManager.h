#ifndef MACROMANAGER_H
#define MACROMANAGER_H

#include "MacroRecorder.h"

#include <QObject>

class MacroManager : public QObject
{
    Q_OBJECT

public:
    explicit MacroManager(QObject *parent = nullptr);

    bool isRecording() const;
    void replayCurrentMacro(ScintillaNext *editor);
    void saveCurrentMacro(const QString &macroName);
    const QVector<Macro *> availableMacros() const { return macros; };
    bool hasCurrentMacro() const;

public slots:
    void startRecording(ScintillaNext *editor);
    void stopRecording();

signals:
    void recordingStarted();
    void recordingStopped();

private:
    MacroRecorder recorder;
    Macro *currentMacro = Q_NULLPTR;
    QVector<Macro *> macros;
    bool _isRecording = false;
    bool isCurrentMacroSaved = false;
};

#endif // MACROMANAGER_H
