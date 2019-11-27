// This file is part of Notepad Next.
// Copyright 2019 Justin Dailey
//
// Notepad Next is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Notepad Next is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.


function Component()
{
    component.loaded.connect(this, function() {
        if (installer.isInstaller()) {
            if (systemInfo.productType === "windows") {
                installer.addWizardPageItem(component, "InstallOptionsForm", QInstaller.TargetDirectory);
            }
        }
    });
}

Component.prototype.createOperations = function()
{
    // call default implementation to actually install the registeredfile
    component.createOperations();

    var isContextMenuChecked = component.userInterface("InstallOptionsForm").addContextMenu.checked;
    var isDesktopShortcutChecked = component.userInterface("InstallOptionsForm").addDesktopShortcut.checked;
    var isStartMenuShortcutChecked = component.userInterface("InstallOptionsForm").addStartMenuShortcut.checked;
    if (systemInfo.productType === "windows") {
        // Right-click context menu
        if (isContextMenuChecked) {
            component.addElevatedOperation("GlobalConfig", "HKEY_CLASSES_ROOT\\*\\shell\\notepadnext", "Default", "Open with Notepad Next");
            component.addElevatedOperation("GlobalConfig", "HKEY_CLASSES_ROOT\\*\\shell\\notepadnext", "icon", "\"@TargetDir@\\NotepadNext.exe\"");
            component.addElevatedOperation("GlobalConfig", "HKEY_CLASSES_ROOT\\*\\shell\\notepadnext\\command", "Default", "\"@TargetDir@\\NotepadNext.exe\" \"%1\"");
        }

        // Create desktop shortcut
        if (isDesktopShortcutChecked) {
            component.addOperation("CreateShortcut",
                "@TargetDir@/NotepadNext.exe",
                "@DesktopDir@/NotepadNext.lnk",
                "workingDirectory=@TargetDir@",
                "iconPath=@TargetDir@/NotepadNext.exe",
                "iconId=0");
        }

        // Create start menu shortcut
        if (isStartMenuShortcutChecked) {
            component.addOperation("CreateShortcut",
                "@TargetDir@/NotepadNext.exe",
                "@StartMenuDir@/NotepadNext.lnk",
                "workingDirectory=@TargetDir@",
                "iconPath=@TargetDir@/NotepadNext.exe",
                "iconId=0"
            );
            component.addOperation("CreateShortcut",
                "@TargetDir@/maintenancetool.exe",
                "@StartMenuDir@/Uninstall NotepadNext.lnk",
                "workingDirectory=@TargetDir@",
                "iconPath=@TargetDir@/NotepadNext.exe",
                "iconId=0"
            );
        }
    }
}
