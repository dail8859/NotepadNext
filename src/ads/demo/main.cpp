#include <MainWindow.h>
#include <QString>
#include <QFile>
#include <QApplication>
#include <QDebug>

#include <memory>



void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }

    fflush(stderr);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if QT_VERSION >= 0x050600
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	std::shared_ptr<int> b;
	QApplication a(argc, argv);
	a.setApplicationName("Advanced Docking System Demo");
	a.setQuitOnLastWindowClosed(true);

	QFile StyleSheetFile(":/adsdemo/app.css");
	StyleSheetFile.open(QIODevice::ReadOnly);
	QTextStream StyleSheetStream(&StyleSheetFile);
	a.setStyleSheet(StyleSheetStream.readAll());
	StyleSheetFile.close();

	qInstallMessageHandler(myMessageOutput);
	qDebug() << "Message handler test";

	CMainWindow mw;
	mw.show();
	return a.exec();
}
