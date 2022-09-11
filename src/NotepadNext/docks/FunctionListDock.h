// docks/FunctionListDock.h
// started 20220903 afo@wega131

#ifndef FunctionListDock_h
#define FunctionListDock_h

#include <QDockWidget>

namespace Ui { class FunctionListDock; }

class FunctionListDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit FunctionListDock(QWidget *parent = nullptr);
    ~FunctionListDock();

	void update(const QString& path);

	void setCtagsCmd(const QString& ctagsCmd) { m_ctagsCmd = ctagsCmd; }

signals:
	void itemActivated(int lineNo);

private:
    Ui::FunctionListDock *ui = nullptr;
	QString m_currentPath;
	QString m_ctagsCmd;
};

#endif // FunctionListDock_h
