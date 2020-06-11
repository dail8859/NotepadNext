import os
import sys

from PyQt5 import uic
from PyQtAds import QtAds

UI_FILE = os.path.join(os.path.dirname(__file__), 'StatusDialog.ui')
StatusDialogUI, StatusDialogBase = uic.loadUiType(UI_FILE)

class CStatusDialog(StatusDialogUI, StatusDialogBase):
    
    def __init__(self, dock_manager: QtAds.CDockManager, parent=None):
        super().__init__(parent)

        self.setupUi(self)
        
        self.dock_manager = dock_manager
        self.dock_widgets = self.dock_manager.dockWidgetsMap()
        
        for key, widget in self.dock_widgets.items():
            self.dockWidgetsComboBox.addItem(key, widget)
            
    def on_dockWidgetsComboBox_currentIndexChanged(self, index: int):
        if not isinstance(index, int):
            return
        if index < 0:
            return
        
        dock_widget = self.dockWidgetsComboBox.currentData()
        self.isClosedCheckBox.setChecked(dock_widget.isClosed())
        self.isFloatingCheckBox.setChecked(dock_widget.isFloating())
        self.tabbedCheckBox.setChecked(dock_widget.isTabbed())
        self.isCurrentTabCheckBox.setChecked(dock_widget.isCurrentTab())
        self.closableCheckBox.setChecked(dock_widget.features() & QtAds.CDockWidget.DockWidgetClosable)
        self.movableCheckBox.setChecked(dock_widget.features() & QtAds.CDockWidget.DockWidgetMovable)
        self.floatableCheckBox.setChecked(dock_widget.features() & QtAds.CDockWidget.DockWidgetFloatable)
        self.deleteOnCloseCheckBox.setChecked(dock_widget.features() & QtAds.CDockWidget.DockWidgetDeleteOnClose)
        self.customCloseHandlingCheckBox.setChecked(dock_widget.features() & QtAds.CDockWidget.CustomCloseHandling)
