// ListDialog.h
// started 20220816 afo@fly130

#ifndef ListDialog_h
#define ListDialog_h

#include <QDialog>

class QListWidgetItem;

namespace Ui {
class ListDialog;
}

class ListDialog : public QDialog
{
	Q_OBJECT
public:
    explicit ListDialog(QWidget *parent = nullptr);
    ~ListDialog();

	void clear();

	int count() const;

	void addLine(const QString& str);

	void addList(const QStringList& strList);

	void setList(const QStringList& strList);

	QString getCurrentLine() const;

	void setCurrentLine(const QString& str);

	void setButtonLabels(const QString& okLabel, const QString& applyLabel = "", const QString& cancelLabel = "");

public slots:
	void slotItemActivated(QListWidgetItem*);
	void slotItemDoubleClicked(QListWidgetItem*);

	int exec() override;

signals:
	void clickedApply(const QString& str);

private:
    Ui::ListDialog *ui;
	QString m_currentLine;
};

#endif // ListDialog_h
