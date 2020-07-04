#include "../../examples/simple/MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
	m_DockManager = new ads::CDockManager(this);

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
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);*/

    ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
	m_DockManager1 = new ads::CDockManager(this);

	// Create example content label - this can be any application specific
	// widget
	ads::CDockWidget* DockWidget;
	{
		QLabel* l = new QLabel();
		l->setWordWrap(true);
		l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

		// Create a dock widget with the title Label 1 and set the created label
		// as the dock widget content
		DockWidget = new ads::CDockWidget("Label 1");
		DockWidget->setWidget(l);
	}

	ads::CDockWidget* DockWidget2;
	{
		QLabel* l = new QLabel();
		l->setWordWrap(true);
		l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

		// Create a dock widget with the title Label 1 and set the created label
		// as the dock widget content
		DockWidget2 = new ads::CDockWidget("Label 2");
		DockWidget2->setWidget(l);
	}


	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	ui->menuView->addAction(DockWidget->toggleViewAction());

	// Add the dock widget to the top dock widget area
	m_DockManager1->addDockWidget(ads::TopDockWidgetArea, DockWidget);


	auto funcRemoveFirstManager = [=]()
	{
		m_DockManager1->removeDockWidget(DockWidget);

		delete m_DockManager1;
		m_DockManager1 = nullptr;
	};
	QTimer::singleShot(3000, funcRemoveFirstManager);

	auto funcAddSecondManager = [=]()
	{
		m_DockManager2 = new ads::CDockManager(this);

		m_DockManager2->addDockWidget(ads::TopDockWidgetArea, DockWidget);
	};
	QTimer::singleShot(5000, funcAddSecondManager);
}

MainWindow::~MainWindow()
{
    delete ui;
}
