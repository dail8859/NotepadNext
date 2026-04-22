#ifndef PREFERENCESDIALOG2_H
#define PREFERENCESDIALOG2_H

#include <QDialog>

#include <ApplicationSettings.h>
#include <NotepadNextApplication.h>

namespace dev {
    class SettingsManager
    {
    public:
        SettingsManager(ApplicationSettings* actual)
            : mActual(actual)
        {

        }

        void setPreviewMode(bool on)
        {
            if (on)
            {

            }
        }

        bool hasChanges() const;

    private:
        ApplicationSettings* mActual = nullptr;
        ApplicationSettings* mStaged = nullptr;

    };
}

class PreferencesDialog2 : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog2(ApplicationSettings *settings, QWidget *parent = nullptr);
    virtual ~PreferencesDialog2();

private slots:
    void onCategoryChanged(const QModelIndex &index);

    void onOkClicked();
    void onCancelClicked();
    void onResetClicked();

private:
    struct PreferencesDialog2Private;
    PreferencesDialog2Private *p;
};

#endif // PREFERENCESDIALOG2_H
