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

var targetDirectoryPage = null;

function Component()
{
    component.loaded.connect(this, this.installerLoaded);
}

Component.prototype.createOperations = function()
{
    // call default implementation to actually install the registeredfile
    component.createOperations();

    var isContextMenuChecked = component.userInterface("TargetWidget").addContextMenu.checked;
    var isDesktopShortcutChecked = component.userInterface("TargetWidget").addDesktopShortcut.checked;
    var isStartMenuShortcutChecked = component.userInterface("TargetWidget").addStartMenuShortcut.checked;
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

Component.prototype.installerLoaded = function()
{
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");
    targetDirectoryPage.windowTitle = "Choose Installation Directory";
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);

    gui.pageById(QInstaller.PerformInstallation).entered.connect(this, this.performInstallationPageEntered);
}

Component.prototype.targetChooserClicked = function()
{
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    targetDirectoryPage.targetDirectory.setText(dir);
}

Component.prototype.targetDirectoryChanged = function()
{
    var dir = targetDirectoryPage.targetDirectory.text;
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation will be uninstalled.</p>");
    }
    else if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. Its existing contents will be removed.</p>");
    }
    else {
        targetDirectoryPage.warning.setText("");
    }

    installer.setValue("TargetDir", dir);
}

Component.prototype.performInstallationPageEntered = function()
{
    var dir = installer.value("TargetDir");

    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        console.log("Running uninstaller: " + dir + "/maintenancetool.exe");
        installer.execute(dir + "/maintenancetool.exe", new Array("--script", dir + "/scripts/auto_uninstall.qs"));
    }
}
