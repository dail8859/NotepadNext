#pragma once

#include <QDialog>
#include <memory>
#include <QSettings>
#include <QObject>

class MainWindow;

namespace Ui {
class RunConfigDialog;
}

class RunConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunConfigDialog(QSettings *runConf, MainWindow *parent = nullptr);
    ~RunConfigDialog() override;

private slots:
    void selectFile();
    void selectFolder();
    void upsertEntry();
    void deleteEntry();
    void loadRowIntoInputs();
    void onRun();
    void onSave();

private:
    void loadConfig();
    int findRowByKey(const QString &key) const;
    void setRow(int row, const QString &key, const QString &value);
    void insertIntoCommand(const QString &path);

    std::unique_ptr<Ui::RunConfigDialog> ui;
	QSettings *runConfSettings;
};
