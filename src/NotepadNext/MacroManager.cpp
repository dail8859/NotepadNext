#include "MacroManager.h"

#include <QSettings>
#include <QCoreApplication>

MacroManager::MacroManager(QObject *parent) :
    QObject{parent}
{
    QSettings settings;

    int size = settings.beginReadArray("Macros");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Macro *m = new Macro(settings.value("Macro").value<Macro>());

        if (m->size() == 0) {
            qWarning("MacroManager: Skipping invalid Macro");
            delete m;
        }
        else {
            macros.append(m);
        }
    }
    settings.endArray();

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [=]() {
        QSettings settings;

        settings.remove("Macros");

        settings.beginWriteArray("Macros");
        for (int i = 0; i < macros.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("Macro", QVariant::fromValue(*macros.at(i)));
        }
        settings.endArray();
    });
}

void MacroManager::startRecording(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);
    Q_ASSERT(_isRecording == false);

    _isRecording = true;

    recorder.startRecording(editor);

    emit recordingStarted();
}

void MacroManager::stopRecording()
{
    qInfo(Q_FUNC_INFO);
    Q_ASSERT(_isRecording == true);

    _isRecording = false;

    Macro *m = recorder.stopRecording();

    if (m->size() == 0) {
        // If there were no actions recorded, delete it
        delete m;
    }
    else {
        if (isCurrentMacroSaved == false) {
            // The previous current macro wasn't saved and we are getting ready to point to something else, delete it
            delete currentMacro;
        }

        isCurrentMacroSaved = false;
        currentMacro = m;
    }

    emit recordingStopped();
}

void MacroManager::replayCurrentMacro(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    currentMacro->replay(editor);
}

void MacroManager::saveCurrentMacro(const QString &macroName)
{
    qInfo(Q_FUNC_INFO);

    isCurrentMacroSaved = true;

    currentMacro->setName(macroName);
    macros.append(currentMacro);
}

bool MacroManager::hasCurrentMacro() const
{
    return currentMacro != Q_NULLPTR;
}
