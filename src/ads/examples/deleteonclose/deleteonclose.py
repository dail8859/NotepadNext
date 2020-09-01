import sys

from PyQtAds import QtAds
from PyQt5.QtGui import QCloseEvent
from PyQt5.QtCore import (qDebug, pyqtSlot, QObject, pyqtSignal)
from PyQt5.QtWidgets import (QMainWindow, QAction, QTextEdit, QApplication,
                             QMenuBar)


class MainWindow(QMainWindow):
    dock_manager = None
    
    def closeEvent(self, event: QCloseEvent):
        super().closeEvent(event)
        if self.dock_manager is not None:
            self.dock_manager.deleteLater()
    
    def setDockManager(self, dock_manager: QtAds.CDockManager):
        self.dock_manager = dock_manager
        

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainWindow()
    
    QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.FocusHighlighting, True)
    QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.AllTabsHaveCloseButton, True)
    dock_manager = QtAds.CDockManager(w)
    w.setDockManager(dock_manager)
    
    count = 0
    
    def on_focused_dock_widget_changed(old: QtAds.CDockWidget, now: QtAds.CDockWidget):
        global count
        qDebug( "{:d} CDockManager::focusedDockWidgetChanged old: {} now: {} visible: {}".format(
            count,
            old.objectName() if old else "-",
            now.objectName(),
            now.isVisible()))
        count += 1
        now.widget().setFocus()
        
    dock_manager.focusedDockWidgetChanged.connect(on_focused_dock_widget_changed)
    
    action = QAction("New Delete On Close", w)
    w.menuBar().addAction(action)
    
    i = 0
    def on_action_triggered():
        global i
        dw = QtAds.CDockWidget("test doc {:d}".format(i))
        i += 1
        editor = QTextEdit("lorem ipsum...", dw)
        dw.setWidget(editor)
        dw.setFeature(QtAds.CDockWidget.DockWidgetDeleteOnClose, True)
        area = dock_manager.addDockWidgetTab(QtAds.CenterDockWidgetArea, dw)
        qDebug("doc dock widget created! {} {}".format(dw, area))
    action.triggered.connect(on_action_triggered)
    
    action = QAction("New", w)
    w.menuBar().addAction(action)
    def on_action2_triggered():
        global i
        dw = QtAds.CDockWidget("test {:d}".format(i))
        i += 1
        editor = QTextEdit("lorem ipsum...", dw)
        dw.setWidget(editor)
        area = dock_manager.addDockWidgetTab(QtAds.CenterDockWidgetArea, dw)
        qDebug("dock widget created! {} {}".format(dw, area))
    action.triggered.connect(on_action2_triggered)
    
    w.show()
    app.exec_()