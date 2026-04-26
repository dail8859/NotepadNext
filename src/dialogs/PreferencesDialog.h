#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class ApplicationSettings;

class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(ApplicationSettings *settings, QWidget *parent = nullptr);
    virtual ~PreferencesDialog();

protected:
    virtual void showEvent(QShowEvent *event) override;

private slots:
    void onCategoryChanged(const QModelIndex &index);

    void onOkClicked();
    void onCancelClicked();
    void onResetClicked();

private:
    struct PreferencesDialogPrivate;
    PreferencesDialogPrivate *p;
};

#endif // PREFERENCESDIALOG_H
