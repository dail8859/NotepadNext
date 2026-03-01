/*
 * This file is part of Notepad Next.
 * Copyright 2024 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MacroManager.h"
#include "ApplicationSettings.h"

MacroManager::MacroManager(QObject *parent) :
    QObject{parent}
{
    qInfo(Q_FUNC_INFO);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaTypeStreamOperators<Macro>("Macro");
#else
    // HACK: For some reason this is required to make QVariant recognize it as a valid type
    // see https://stackoverflow.com/q/70974383
    QMetaType::fromType<Macro>().hasRegisteredDataStreamOperators();
#endif

    loadSettings();
}

MacroManager::~MacroManager()
{
    saveSettings();
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

void MacroManager::loadSettings()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    int size = settings.beginReadArray("Macros");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        if (settings.value("Macro").canConvert<Macro>()) {
            Macro *m = new Macro(settings.value("Macro").value<Macro>());
            macros.append(m);
        }
        else {
            qWarning("MacroManager: Skipping invalid Macro");
        }
    }
    settings.endArray();
}

void MacroManager::saveSettings() const
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.remove("Macros");

    if (macros.size() > 0) {
        settings.beginWriteArray("Macros");
        for (int i = 0; i < macros.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("Macro", QVariant::fromValue(*macros.at(i)));
        }
        settings.endArray();
    }
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

bool MacroManager::hasCurrentUnsavedMacro() const
{
    return currentMacro != Q_NULLPTR && isCurrentMacroSaved == false;
}
