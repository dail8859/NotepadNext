#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
	QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
	Layout->setContentsMargins(QMargins(0, 0, 0, 0));
	m_DockManager = new ads::CDockManager(ui->dockContainer);
	Layout->addWidget(m_DockManager);

	// Create example content label - this can be any application specific
	// widget
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	// Create a dock widget with the title Label 1 and set the created label
	// as the dock widget content
	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(l);

	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	ui->menuView->addAction(DockWidget->toggleViewAction());


	// Add the dock widget to the top dock widget area
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);

	// Create an example editor
	QPlainTextEdit* te = new QPlainTextEdit();
	te->setPlaceholderText("Please enter your text here into this QPlainTextEdit...");
	DockWidget = new ads::CDockWidget("Editor 1");
	ui->menuView->addAction(DockWidget->toggleViewAction());
	m_DockManager->addDockWidget(ads::BottomDockWidgetArea, DockWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if (m_DockManager)
    {
        m_DockManager->deleteLater();
    }
}
