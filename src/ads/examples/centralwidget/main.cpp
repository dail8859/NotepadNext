#include <mainwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWindow w;
    w.show();
    return a.exec();
}
