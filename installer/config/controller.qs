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


function Controller()
{
    console.log("OS: " + systemInfo.prettyProductName);
    console.log("Kernel: " + systemInfo.kernelType + "/" + systemInfo.kernelVersion);
    console.log("CPU Architecture: " +  systemInfo.currentCpuArchitecture);

    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);

    installer.uninstallationFinished.connect(this, function() {
        var key_exists =installer.execute("reg", new Array("QUERY", "HKEY_CLASSES_ROOT\\*\\shell\\notepadnext", "/VE"))[1];
        if (key_exists == 0) {
            console.log("Removing key for context menu");
            if (installer.gainAdminRights()) {
                installer.execute("reg", new Array("DELETE", "HKEY_CLASSES_ROOT\\*\\shell\\notepadnext", "/f"));
            }
            else {
                console.log("Couldn't gain admin rights");
            }
        }
    });
}
