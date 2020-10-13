import os
import sys

from PyQt5 import uic
from PyQt5.QtCore import Qt, QTimer, QDir
from PyQt5.QtGui import QCloseEvent
from PyQt5.QtWidgets import (QApplication, QLabel, QCalendarWidget, QFrame, QTreeView,
                             QTableWidget, QFileSystemModel)

from PyQtAds import QtAds

UI_FILE = os.path.join(os.path.dirname(__file__), 'mainwindow.ui')
MainWindowUI, MainWindowBase = uic.loadUiType(UI_FILE)


class MainWindow(MainWindowUI, MainWindowBase):

    def __init__(self, parent=None):
        super().__init__(parent)

        self.setupUi(self)
 
        QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.OpaqueSplitterResize, True)
        QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.XmlCompressionEnabled, False)
        self.dock_manager = QtAds.CDockManager(self)
        
        # Set central widget
        calendar = QCalendarWidget()
        central_dock_widget = QtAds.CDockWidget("CentralWidget")
        central_dock_widget.setWidget(calendar)
        central_dock_area = self.dock_manager.setCentralWidget(central_dock_widget)
        central_dock_area.setAllowedAreas(QtAds.DockWidgetArea.OuterDockAreas)
        
        # create other dock widgets
        file_tree = QTreeView()
        file_tree.setFrameShape(QFrame.NoFrame)
        file_model = QFileSystemModel(file_tree)
        file_model.setRootPath(QDir.currentPath())
        file_tree.setModel(file_model)
        data_dock_widget = QtAds.CDockWidget("File system")
        data_dock_widget.setWidget(file_tree)
        data_dock_widget.resize(150, 250)
        data_dock_widget.setMinimumSize(100, 250)
        file_area = self.dock_manager.addDockWidget(QtAds.DockWidgetArea.LeftDockWidgetArea, data_dock_widget, central_dock_area)
        self.menuView.addAction(data_dock_widget.toggleViewAction())

        table = QTableWidget()
        table.setColumnCount(3)
        table.setRowCount(10)
        table_dock_widget = QtAds.CDockWidget("Table")
        table_dock_widget.setWidget(table)
        table_dock_widget.setMinimumSizeHintMode(QtAds.CDockWidget.MinimumSizeHintFromDockWidget)
        table_dock_widget.resize(250, 150)
        table_dock_widget.setMinimumSize(200, 150)
        self.dock_manager.addDockWidget(QtAds.DockWidgetArea.BottomDockWidgetArea, table_dock_widget, file_area)
        self.menuView.addAction(table_dock_widget.toggleViewAction())

        properties_table = QTableWidget()
        properties_table.setColumnCount(3)
        properties_table.setRowCount(10)
        properties_dock_widget = QtAds.CDockWidget("Properties")
        properties_dock_widget.setWidget(properties_table)
        properties_dock_widget.setMinimumSizeHintMode(QtAds.CDockWidget.MinimumSizeHintFromDockWidget)
        properties_dock_widget.resize(250, 150)
        properties_dock_widget.setMinimumSize(200,150)
        self.dock_manager.addDockWidget(QtAds.DockWidgetArea.RightDockWidgetArea, properties_dock_widget, central_dock_area)
        self.menuView.addAction(properties_dock_widget.toggleViewAction())


if __name__ == '__main__':
    app = QApplication(sys.argv)
    
    w = MainWindow()
    w.show()
    app.exec_()
