#ifndef LANGUAGEINSPECTORDOCK_H
#define LANGUAGEINSPECTORDOCK_H

#include <QDockWidget>

class MainWindow;
class ScintillaNext;

namespace Ui {
class LanguageInspectorDock;
}

class LanguageInspectorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit LanguageInspectorDock(MainWindow *parent);
    ~LanguageInspectorDock();

private slots:
    void updateInformation(ScintillaNext *editor);

private:
    Ui::LanguageInspectorDock *ui;

    void updateLanguageName(ScintillaNext *editor);
    void updateKeywordInfo(ScintillaNext *editor);
    void updateStyleInfo(ScintillaNext *editor);
};

#endif // LANGUAGEINSPECTORDOCK_H
