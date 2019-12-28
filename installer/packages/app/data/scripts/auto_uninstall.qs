// Controller script to pass to the uninstaller to get it to run automatically.
// It's passed to the maintenance tool during installation if there is already an
// installation present with: <target dir>/maintenancetool.exe --script=<target dir>/scripts/auto_uninstall.qs.
// This is required so that the user doesn't have to see/deal with the uninstaller in the middle of
// an installation.

function Controller()
{
    gui.clickButton(buttons.NextButton);
    gui.clickButton(buttons.NextButton);

    installer.uninstallationFinished.connect(this, this.uninstallationFinished);
}

Controller.prototype.uninstallationFinished = function()
{
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function()
{
    gui.clickButton(buttons.FinishButton);
}