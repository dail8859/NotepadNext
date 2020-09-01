import os
import sys

from PyQt5 import uic
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QCloseEvent
from PyQt5.QtWidgets import QApplication, QLabel

from PyQtAds import QtAds

UI_FILE = os.path.join(os.path.dirname(__file__), 'MainWindow.ui')
MainWindowUI, MainWindowBase = uic.loadUiType(UI_FILE)


class MainWindow(MainWindowUI, MainWindowBase):

    def __init__(self, parent=None):
        super().__init__(parent)

        self.setupUi(self)
 
        # Create the dock manager. Because the parent parameter is a QMainWindow
        # the dock manager registers itself as the central widget.
        self.dock_manager1 = QtAds.CDockManager(self)
        
        # Create example content label - this can be any application specific
        # widget
        l = QLabel()
        l.setWordWrap(True)
        l.setAlignment(Qt.AlignTop | Qt.AlignLeft);
        l.setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")

        # Create a dock widget with the title Label 1 and set the created label
        # as the dock widget content
        dock_widget = QtAds.CDockWidget("Label 1")
        dock_widget.setWidget(l)
        
        l = QLabel()
        l.setWordWrap(True)
        l.setAlignment(Qt.AlignTop | Qt.AlignLeft);
        l.setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")

        # Create a dock widget with the title Label 1 and set the created label
        # as the dock widget content
        dock_widget2 = QtAds.CDockWidget("Label 2")
        dock_widget2.setWidget(l)

        # Add the toggleViewAction of the dock widget to the menu to give
        # the user the possibility to show the dock widget if it has been closed
        self.menuView.addAction(dock_widget.toggleViewAction())

        # Add the dock widget to the top dock widget area
        self.dock_manager1.addDockWidget(QtAds.TopDockWidgetArea, dock_widget)
        
        def remove_first_manager():
            self.dock_manager1.removeDockWidget(dock_widget)
            del self.dock_manager1
        QTimer.singleShot(3000, remove_first_manager)
            
        def add_second_manager():
            self.dock_manager2 = QtAds.CDockManager(self)
            self.dock_manager2.addDockWidget(QtAds.TopDockWidgetArea, dock_widget)
        QTimer.singleShot(5000, add_second_manager)
        
    def closeEvent(self, event: QCloseEvent):
        super().closeEvent(event)
        
        if hasattr(self, 'dock_manager1'):
            self.dock_manager1.deleteLater()
            
        if hasattr(self, 'dock_manager2'):
            self.dock_manager2.deleteLater()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    
    w = MainWindow()
    w.show()
    app.exec_()
