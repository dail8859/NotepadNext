#ifndef LANGUAGEINSPECTORDOCK_H
#define LANGUAGEINSPECTORDOCK_H

#include <QDockWidget>

class MainWindow;
class DockedEditor;

namespace Ui {
class LanguageInspectorDock;
}

class LanguageInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit LanguageInspectorDock(MainWindow *parent);
    ~LanguageInspectorDock();

private:
    Ui::LanguageInspectorDock *ui;
    DockedEditor *dockedEditor;
};

#endif // LANGUAGEINSPECTORDOCK_H
