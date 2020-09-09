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
        self.dock_manager = QtAds.CDockManager(self)
        
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
        

        # Add the toggleViewAction of the dock widget to the menu to give
        # the user the possibility to show the dock widget if it has been closed
        self.menuView.addAction(dock_widget.toggleViewAction())

        # Add the dock widget to the top dock widget area
        self.dock_manager.addDockWidget(QtAds.TopDockWidgetArea, dock_widget)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    
    w = MainWindow()
    w.show()
    app.exec_()
