#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QWidgetAction>
#include <QLabel>
#include <QCalendarWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockAreaTabBar.h"
#include "FloatingDockContainer.h"
#include "DockComponentsFactory.h"

using namespace ads;

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    DockManager = new CDockManager(this);

    // Set central widget
    QCalendarWidget* calendar = new QCalendarWidget();
    CDockWidget* CentralDockWidget = new CDockWidget("CentralWidget");
    CentralDockWidget->setWidget(calendar);
    auto* CentralDockArea = DockManager->setCentralWidget(CentralDockWidget);
    CentralDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);

    // create other dock widgets
    QTreeView* fileTree = new QTreeView();
    fileTree->setFrameShape(QFrame::NoFrame);
    QFileSystemModel* fileModel = new QFileSystemModel(fileTree);
    fileModel->setRootPath(QDir::currentPath());
    fileTree->setModel(fileModel);
    CDockWidget* DataDockWidget = new CDockWidget("File system");
    DataDockWidget->setWidget(fileTree);
    DataDockWidget->resize(150, 250);
    DataDockWidget->setMinimumSize(100, 250);
    auto* fileArea = DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, DataDockWidget, CentralDockArea);
    ui->menuView->addAction(DataDockWidget->toggleViewAction());

    QTableWidget* table = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* TableDockWidget = new CDockWidget("Table");
    TableDockWidget->setWidget(table);
    TableDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    TableDockWidget->resize(250, 150);
    TableDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, TableDockWidget, fileArea);
    ui->menuView->addAction(TableDockWidget->toggleViewAction());

    QTableWidget* propertiesTable = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* PropertiesDockWidget = new CDockWidget("Properties");
    PropertiesDockWidget->setWidget(propertiesTable);
    PropertiesDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    PropertiesDockWidget->resize(250, 150);
    PropertiesDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, PropertiesDockWidget, CentralDockArea);
    ui->menuView->addAction(PropertiesDockWidget->toggleViewAction());
}

CMainWindow::~CMainWindow()
{
    delete ui;
}

