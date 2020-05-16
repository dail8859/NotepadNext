import datetime
import logging
import os
import sys

from PyQt5 import uic
from PyQt5.QtCore import (QCoreApplication, QDir, Qt, QSettings, QSignalBlocker,
                          QRect, QPoint, qDebug, qInstallMessageHandler,
                          QtDebugMsg, QtInfoMsg, QtWarningMsg,
                          QtCriticalMsg, QtFatalMsg)
from PyQt5.QtGui import (QGuiApplication, QIcon, QCloseEvent)
from PyQt5.QtWidgets import (QCalendarWidget, QFileSystemModel, QFrame, QLabel,
                            QMenu, QTreeView, QAction, QWidgetAction,
                            QComboBox, QStyle, QSizePolicy, QInputDialog, QMenu,
                            QToolButton, QWidget, QPlainTextEdit,
                            QTableWidget, QTableWidgetItem, QApplication,
                            QMessageBox)
try:
    from PyQt5.QAxContainer import QAxWidget
except ImportError:
    ACTIVEX_AVAILABLE = False
else:
    ACTIVEX_AVAILABLE = True

from PyQtAds import QtAds

import rc  # pyrcc5 demo.qrc -o rc.py

UI_FILE = os.path.join(os.path.dirname(__file__), 'mainwindow.ui')
MainWindowUI, MainWindowBase = uic.loadUiType(UI_FILE)


class _State:
    label_count = 0
    calendar_count = 0
    file_system_count = 0
    editor_count = 0
    table_count = 0
    activex_count = 0
    

def features_string(dock_widget: QtAds.CDockWidget) -> str:
    '''Function returns a features string with closable (c), movable (m) and floatable (f)
       features. i.e. The following string is for a not closable but movable and floatable
       widget: c- m+ f+'''

    f = dock_widget.features()
    closable = f & QtAds.CDockWidget.DockWidgetClosable
    movable = f & QtAds.CDockWidget.DockWidgetMovable
    floatable = f &QtAds.CDockWidget.DockWidgetFloatable
    
    return "c{} m{} f{}".format("+" if closable else "-",
                                "+" if movable else "-",
                                "+" if floatable else "-")


def append_feature_string_to_window_title(dock_widget: QtAds.CDockWidget):
    '''Appends the string returned by features_string() to the window title of
       the given DockWidget'''
       
    dock_widget.setWindowTitle(dock_widget.windowTitle() + " ({})".format(features_string(dock_widget)))


def svg_icon(filename: str):
    '''Helper function to create an SVG icon'''
    # This is a workaround, because because in item views SVG icons are not
    # properly scaled an look blurry or pixelate
    icon = QIcon(filename)
    icon.addPixmap(icon.pixmap(92))
    return icon
    

def create_long_text_label_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    label = QLabel()
    label.setWordWrap(True)
    label.setAlignment(Qt.AlignTop | Qt.AlignLeft)
    label.setText('''\
Label {} {} - Lorem ipsum dolor sit amet, consectetuer
adipiscing elit.  Aenean commodo ligula eget dolor. Aenean massa. Cum
sociis natoque penatibus et magnis dis parturient montes, nascetur
ridiculus mus.  Donec quam felis, ultricies nec, pellentesque eu, pretium
quis, sem.  Nulla consequat massa quis enim. Donec pede justo, fringilla
vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut,
imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis
pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi.
Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu,
consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra
quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet.
    '''.format(_State.label_count, datetime.datetime.now().strftime("%H:%M:%S:%f")))
    dock_widget = QtAds.CDockWidget("Label {}".format(_State.label_count))
    _State.label_count += 1
    dock_widget.setWidget(label)

    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget


def create_calendar_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    widget = QCalendarWidget()

    dock_widget = QtAds.CDockWidget("Calendar {}".format(_State.calendar_count))
    _State.calendar_count += 1
    dock_widget.setWidget(widget)
    dock_widget.setToggleViewActionMode(QtAds.CDockWidget.ActionModeShow)
    dock_widget.setIcon(svg_icon(":/adsdemo/images/date_range.svg"))
    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget


def create_file_system_tree_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    widget = QTreeView()
    widget.setFrameShape(QFrame.NoFrame)

    m = QFileSystemModel(widget)
    m.setRootPath(QDir.currentPath())
    widget.setModel(m)

    dock_widget = QtAds.CDockWidget("Filesystem {}".format(_State.file_system_count))
    _State.file_system_count += 1
    dock_widget.setWidget(widget)
    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget
 
 
def create_editor_widget(view_menu: QMenu) -> QtAds.CDockWidget: 
    widget = QPlainTextEdit()
    widget.setPlaceholderText("This is an editor. If you close the editor, it will be "
        "deleted. Enter your text here.")
    widget.setStyleSheet("border: none")
    dock_widget = QtAds.CDockWidget("Editor {}".format(_State.editor_count))
    _State.editor_count += 1
    dock_widget.setWidget(widget)
    dock_widget.setIcon(svg_icon(":/adsdemo/images/edit.svg"))
    dock_widget.setFeature(QtAds.CDockWidget.CustomCloseHandling, True)
    view_menu.addAction(dock_widget.toggleViewAction())

    options_menu = QMenu(dock_widget)
    options_menu.setTitle("Options")
    options_menu.setToolTip(options_menu.title())
    options_menu.setIcon(svg_icon(":/adsdemo/images/custom-menu-button.svg"))
    menu_action = options_menu.menuAction()
    # The object name of the action will be set for the QToolButton that
    # is created in the dock area title bar. You can use this name for CSS
    # styling
    menu_action.setObjectName("options_menu")
    dock_widget.setTitleBarActions([options_menu.menuAction()])
    a = options_menu.addAction("Clear Editor")
    a.triggered.connect(widget.clear)

    return dock_widget


def create_table_widget(view_menu: QMenu) -> QtAds.CDockWidget: 
    widget = QTableWidget()
    dock_widget = QtAds.CDockWidget("Table {}".format(_State.table_count))
    _State.table_count += 1
    COLCOUNT = 5
    ROWCOUNT = 30
    widget.setColumnCount(COLCOUNT)
    widget.setRowCount(ROWCOUNT)
    for col in range(ROWCOUNT):
        widget.setHorizontalHeaderItem(col, QTableWidgetItem("Col {}".format(col+1)))
        for row in range(ROWCOUNT):
            widget.setItem(row, col, QTableWidgetItem("T {:}-{:}".format(row+1, col+1)))
    
    dock_widget.setWidget(widget)
    dock_widget.setIcon(svg_icon(":/adsdemo/images/grid_on.svg"))
    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget
    
    
if ACTIVEX_AVAILABLE:
    def create_activex_widget(view_menu: QMenu, parent: QWidget = None) -> QtAds.CDockWidget: 
        widget = QAxWidget("{6bf52a52-394a-11d3-b153-00c04f79faa6}", parent)
        dock_widget = QtAds.CDockWidget("Active X {}".format(_State.activex_count))
        _State.activex_count += 1
        dock_widget.setWidget(widget)
        view_menu.addAction(dock_widget.toggleViewAction())
        return dock_widget
    
    
class CustomComponentsFactory(QtAds.CDockComponentsFactory):

    def createDockAreaTitleBar(self, dock_area: QtAds.CDockAreaWidget) -> QtAds.CDockAreaTitleBar:
        title_bar = QtAds.CDockAreaTitleBar(dock_area)
        custom_button = QToolButton(dock_area)
        custom_button.setToolTip("Help")
        custom_button.setIcon(svg_icon(":/adsdemo/images/help_outline.svg"))
        custom_button.setAutoRaise(True)
        index = title_bar.indexOf(title_bar.button(QtAds.TitleBarButtonTabsMenu))
        title_bar.insertWidget(index + 1, custom_button)
        return title_bar



class MainWindow(MainWindowUI, MainWindowBase):
    save_perspective_action: QAction
    perspective_list_action: QWidgetAction
    perspective_combo_box: QComboBox
    dock_manager: QtAds.CDockManager

    def __init__(self, parent=None):
        super().__init__(parent)
        self.save_perspective_action = None
        self.perspective_list_action = None
        self.perspective_combo_box = None
        self.dock_manager = None

        self.setupUi(self)
        self.create_actions()
        
        # uncomment the following line if the tab close button should be
        # a QToolButton instead of a QPushButton
        # QtAds.CDockManager.setConfigFlags(QtAds.CDockManager.configFlags() | QtAds.CDockManager.TabCloseButtonIsToolButton)
        
        # uncomment the following line if you want a fixed tab width that does
        # not change if the visibility of the close button changes
        # QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.RetainTabSizeWhenCloseButtonHidden, True)

        # uncomment the follwing line if you want to use non opaque undocking and splitter
        # movements
        # QtAds.CDockManager.setConfigFlags(QtAds.CDockManager.DefaultNonOpaqueConfig)

        # Now create the dock manager and its content
        self.dock_manager = QtAds.CDockManager(self)
        
        # Uncomment the following line to have the old style where the dock
        # area close button closes the active tab
        # QtAds.CDockManager.setConfigFlags(QtAds.CDockManager.DockAreaHasCloseButton
                                          # | QtAds.CDockManager.DockAreaCloseButtonClosesTab)
        self.perspective_combo_box.activated[str].connect(self.dock_manager.openPerspective)
        
        self.create_content()
        # Default window geometry - center on screen
        self.resize(1280, 720)
        self.setGeometry(QStyle.alignedRect(
            Qt.LeftToRight, Qt.AlignCenter, self.frameSize(),
            QGuiApplication.primaryScreen().availableGeometry()))
        
        # self.restore_state()
        self.restore_perspectives() 

    def create_content(self):
        # Test container docking
        view_menu = self.menuView
        dock_widget = create_calendar_dock_widget(view_menu)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetClosable, False)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetMovable, False)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetFloatable, False)
        special_dock_area = self.dock_manager.addDockWidget(QtAds.LeftDockWidgetArea, dock_widget)
        
        # For this Special Dock Area we want to avoid dropping on the center of it (i.e. we don't want this widget to be ever tabbified):
        special_dock_area.setAllowedAreas(QtAds.OuterDockAreas)
        # special_dock_area.setAllowedAreas(QtAds.LeftDockWidgetArea | QtAds.RightDockWidgetArea) # just for testing
        
        self.dock_manager.addDockWidget(QtAds.LeftDockWidgetArea, create_long_text_label_dock_widget(view_menu))
        file_system_widget = create_file_system_tree_dock_widget(view_menu)
        tool_bar = file_system_widget.createDefaultToolBar()
        tool_bar.addAction(self.actionSaveState)
        tool_bar.addAction(self.actionRestoreState)
        file_system_widget.setFeature(QtAds.CDockWidget.DockWidgetFloatable, False)
        append_feature_string_to_window_title(file_system_widget)
        self.dock_manager.addDockWidget(QtAds.BottomDockWidgetArea, file_system_widget)
        
        file_system_widget = create_file_system_tree_dock_widget(view_menu)
        tool_bar = file_system_widget.createDefaultToolBar()
        tool_bar.addAction(self.actionSaveState)
        tool_bar.addAction(self.actionRestoreState)
        file_system_widget.setFeature(QtAds.CDockWidget.DockWidgetMovable, False)
        file_system_widget.setFeature(QtAds.CDockWidget.DockWidgetFloatable, False)
        append_feature_string_to_window_title(file_system_widget)
        
        # Test custom factory - we inject a help button into the title bar
        self.factory = CustomComponentsFactory()
        QtAds.CDockComponentsFactory.setFactory(self.factory)
        top_dock_area = self.dock_manager.addDockWidget(QtAds.TopDockWidgetArea, file_system_widget)
        QtAds.CDockComponentsFactory.resetDefaultFactory()


        # We create a calendar widget and clear all flags to prevent the dock area
        # from closing
        dock_widget = create_calendar_dock_widget(view_menu)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetClosable, False)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetMovable, False)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetFloatable, False)
        dock_widget.setTabToolTip("Tab ToolTip\nHodie est dies magna")
        dock_area = self.dock_manager.addDockWidget(QtAds.CenterDockWidgetArea, dock_widget, top_dock_area)

        # Now we add a custom button to the dock area title bar that will create
        # new editor widgets when clicked
        custom_button = QToolButton(dock_area)
        custom_button.setToolTip("Create Editor")
        custom_button.setIcon(svg_icon(":/adsdemo/images/plus.svg"))
        custom_button.setAutoRaise(True)
        title_bar = dock_area.titleBar()
        index = title_bar.indexOf(title_bar.tabBar())
        title_bar.insertWidget(index + 1, custom_button)
        def on_button_clicked():
            dock_widget = create_editor_widget(self.menuView)
            dock_widget.setFeature(QtAds.CDockWidget.DockWidgetDeleteOnClose, True)
            self.dock_manager.addDockWidgetTabToArea(dock_widget, dock_area)
            dock_widget.closeRequested.connect(self.on_editor_close_requested)
        custom_button.clicked.connect(on_button_clicked)

        # Test dock area docking
        right_dock_area = self.dock_manager.addDockWidget(
            QtAds.RightDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), top_dock_area)
        self.dock_manager.addDockWidget(
            QtAds.TopDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)

        bottom_dock_area = self.dock_manager.addDockWidget(
            QtAds.BottomDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)

        self.dock_manager.addDockWidget(
            QtAds.CenterDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)
        self.dock_manager.addDockWidget(
            QtAds.CenterDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), bottom_dock_area)
            
        action = self.menuView.addAction("Set {} floating".format(dock_widget.windowTitle()))
        action.triggered.connect(dock_widget.setFloating)
        
        if ACTIVEX_AVAILABLE:
            flags = self.dock_manager.configFlags()
            if flags & QtAds.CDockManager.OpaqueUndocking:
                self.dock_manager.addDockWidget(QtAds.CenterDockWidgetArea,
                    create_activex_widget(view_menu), right_dock_area)
                    
        for dock_widget in self.dock_manager.dockWidgetsMap().values():
            dock_widget.viewToggled.connect(self.on_view_toggled)
            dock_widget.visibilityChanged.connect(self.on_view_visibility_changed)

    def create_actions(self):
        self.toolBar.addAction(self.actionSaveState)
        self.toolBar.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
        self.actionSaveState.setIcon(svg_icon(":/adsdemo/images/save.svg"))
        self.toolBar.addAction(self.actionRestoreState)
        self.actionRestoreState.setIcon(svg_icon(":/adsdemo/images/restore.svg"))
        
        self.save_perspective_action = QAction("Create Perspective", self)
        self.save_perspective_action.setIcon(svg_icon(":/adsdemo/images/picture_in_picture.svg"))
        self.save_perspective_action.triggered.connect(self.save_perspective)
        self.perspective_list_action = QWidgetAction(self)
        self.perspective_combo_box = QComboBox(self)
        self.perspective_combo_box.setSizeAdjustPolicy(QComboBox.AdjustToContents)
        self.perspective_combo_box.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.perspective_list_action.setDefaultWidget(self.perspective_combo_box)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.perspective_list_action)
        self.toolBar.addAction(self.save_perspective_action)
        
        a = self.toolBar.addAction("Create Editor")
        a.setToolTip("Creates floating dynamic dockable editor windows that are deleted on close")
        a.setIcon(svg_icon(":/adsdemo/images/note_add.svg"))
        a.triggered.connect(self.create_editor)

        a = self.toolBar.addAction("Create Table")
        a.setToolTip("Creates floating dynamic dockable table with millions of entries")
        a.setIcon(svg_icon(":/adsdemo/images/grid_on.svg"))
        a.triggered.connect(self.create_table)
        
    def closeEvent(self, event: QCloseEvent):
        self.save_state()
        super().closeEvent(event)
        
    def on_action_save_state_triggered(state: bool):
        qDebug("MainWindow::on_action_save_state_triggered")
        self.save_state()
        
    def on_action_restore_state_triggered(state: bool):
        qDebug("MainWindow::on_action_restore_state_triggered")
        self.restore_state()
        
    def save_perspective(self):
        perspective_name, ok = QInputDialog.getText(self, "Save perspective",
                                                    "Enter unique name:")
        
        if ok and perspective_name:
            self.dock_manager.addPerspective(perspective_name)
            _ = QSignalBlocker(self.perspective_combo_box)
            self.perspective_combo_box.clear()
            self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())
            self.perspective_combo_box.setCurrentText(perspective_name)
            
            self.save_perspectives()
            
    def on_view_toggled(self, open: bool):
        dock_widget = self.sender()
        if dock_widget is None:
            return

        qDebug("{} view_toggled({})".format(dock_widget.objectName(), open))
        
    def on_view_visibility_changed(self, visible: bool):
        dock_widget = self.sender()
        if dock_widget is None:
            return

        qDebug("{} visibility_changed({})".format(dock_widget.objectName(), visible))
            
    def create_editor(self):
        dock_widget = create_editor_widget(self.menuView)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetDeleteOnClose, True)
        floating_widget = self.dock_manager.addDockWidgetFloating(dock_widget)
        floating_widget.move(QPoint(20, 20))
        dock_widget.closeRequested.connect(self.on_editor_close_requested)
            
    def on_editor_close_requested(self):
        dock_widget = self.sender()
        result = QMessageBox.question(self, "Close Editor",
                "Editor {} contains unsaved changes? Would you like to close it?".format(dock_widget.windowTitle()))
        if result == QMessageBox.Yes:
            dock_widget.closeDockWidget()
            
    def create_table(self):
        dock_widget = create_table_widget(self.menuView)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetDeleteOnClose, True)
        floating_widget = self.dock_manager.addDockWidgetFloating(dock_widget)
        floating_widget.move(QPoint(40, 40))

    def save_state(self):
        '''
        Saves the dock manager state and the main window geometry
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        settings.setValue("mainWindow/Geometry", self.saveGeometry())
        settings.setValue("mainWindow/State", self.saveState())
        settings.setValue("mainWindow/DockingState", self.dock_manager.saveState())

    def restore_state(self):
        '''
        Restores the dock manager state
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        geom = settings.value("mainWindow/Geometry")
        if geom is not None:
            self.restoreGeometry(geom)

        state = settings.value("mainWindow/State")
        if state is not None:
            self.restoreState(state)

        state = settings.value("mainWindow/DockingState")
        if state is not None:
            self.dock_manager.restore_state(state)
            
    def save_perspectives(self):
        '''
        Save the list of perspectives
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        self.dock_manager.savePerspectives(settings)

    def restore_perspectives(self):
        '''
        Restore the perspective listo of the dock manager
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        self.dock_manager.loadPerspectives(settings)
        self.perspective_combo_box.clear()
        self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())

    def save_perspective(self):
        perspective_name, ok = QInputDialog.getText(self, 'Save perspective', 'Enter unique name:')
        if ok and perspective_name:
            self.dock_manager.addPerspective(perspective_name)
            _ = QSignalBlocker(self.perspective_combo_box)
            self.perspective_combo_box.clear()
            self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())
            self.perspective_combo_box.setCurrentText(perspective_name)
            self.save_perspectives()


def my_message_output(type, context, msg):
    if type == QtDebugMsg:
        print("Debug: {} ({}:{}, {})".format(msg, context.file, context.line, context.function))
    elif type == QtInfoMsg:
        print("Info: {} ({}:{}, {})".format(msg, context.file, context.line, context.function))
    elif type == QtWarningMsg:
        print("Warning: {} ({}:{}, {})".format(msg, context.file, context.line, context.function))
    elif type == QtCriticalMsg:
        print("Critical: {} ({}:{}, {})".format(msg, context.file, context.line, context.function))
    elif type == QtFatalMsg:
        print("Fatal: {} ({}:{}, {})".format(msg, context.file, context.line, context.function))


if __name__ == '__main__':
    QCoreApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)
    QGuiApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(True)
    
    with open(os.path.join(os.path.dirname(__file__), "app.css"), "r") as style_sheet_file:
        app.setStyleSheet(style_sheet_file.read())
    
    qInstallMessageHandler(my_message_output)
    qDebug("Message handler test")
    
    mw = MainWindow()
    mw.show()
    app.exec_()
