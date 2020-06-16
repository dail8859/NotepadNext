#include <QMainWindow>
#include <QAction>
#include <QTextEdit>
#include <QApplication>
#include <QMenuBar>
#include "DockManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;

    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    auto dockManager = new ads::CDockManager(&w);

    QAction *action = new QAction("New Delete On Close", &w);
    w.menuBar()->addAction(action);

    int i = 0;
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new ads::CDockWidget(QStringLiteral("test doc %1").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("lorem ipsum..."), dw);
        dw->setWidget(editor);
        dw->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
        auto area = dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);
        qDebug() << "doc dock widget created!" << dw << area;
    });

    action = new QAction("New", &w);
    w.menuBar()->addAction(action);
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new ads::CDockWidget(QStringLiteral("test %1").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("lorem ipsum..."), dw);
        dw->setWidget(editor);
        auto area = dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);
        qDebug() << "dock widget created!" << dw << area;
    });

    w.show();

    return a.exec();
}
