#pragma once

#include <QDialog>
#include <memory>
#include <QSettings>
#include <QObject>
#include <QCompleter>

class MainWindow;

namespace Ui {
class RunConfigDialog;
}

class RunConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RunConfigDialog(MainWindow *parent = nullptr);
    ~RunConfigDialog() override;
    void loadConfig();
	void fillBuildDetails(QString buildNameFromCompleter);
	QString encode(const QString &path);
	QCompleter *runConfigNamesByWorkingDirectory;
	QStringList filteredConfigList;

private slots:
    void upsertEntry();
    void deleteEntry();
    void loadRowIntoInputs();
    void onRun();
    void onSave();
	void onDelete();

private:
    int findRowByKey(const QString &key) const;
    void setRow(int row, const QString &key, const QString &value);
    void insertIntoCommand(const QString &path);

    std::unique_ptr<Ui::RunConfigDialog> ui;
	QSettings *runConfSettings;
};
