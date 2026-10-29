#include "RunConfigDialog.h"
#include "ui_RunConfigDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>

#include "ApplicationSettings.h"
#include "MainWindow.h"

namespace {
const char kFilenameToken[] = "<filename>";
const char kDefaultCommand[] = "go run <filename>";
}

RunConfigDialog::RunConfigDialog(MainWindow *parent)
    : QDialog(parent, Qt::Dialog)
    , ui(new Ui::RunConfigDialog)
{
    ui->setupUi(this);

    connect(ui->upsertButton, &QPushButton::clicked, this, &RunConfigDialog::upsertEntry);
    connect(ui->deleteButton, &QPushButton::clicked, this, &RunConfigDialog::deleteEntry);
    connect(ui->valueEdit, &QLineEdit::returnPressed, this, &RunConfigDialog::upsertEntry);
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &RunConfigDialog::loadRowIntoInputs);
    connect(ui->runButton, &QPushButton::clicked, this, &RunConfigDialog::onRun);
    connect(ui->saveButton, &QPushButton::clicked, this, &RunConfigDialog::onSave);
	connect(ui->buildDeleteButton, &QPushButton::clicked, this, &RunConfigDialog::onDelete);
    connect(ui->doneButton, &QPushButton::clicked, this, &QDialog::reject);

    loadConfig();
}

RunConfigDialog::~RunConfigDialog() = default;

// ---------------------------------------------------------------------------
// Table helpers
// ---------------------------------------------------------------------------

int RunConfigDialog::findRowByKey(const QString &key) const
{
    for (int r = 0; r < ui->table->rowCount(); ++r) {
        if (ui->table->item(r, 0)->text() == key)
            return r;
    }
    return -1;
}

void RunConfigDialog::setRow(int row, const QString &key, const QString &value)
{
    ui->table->setItem(row, 0, new QTableWidgetItem(key));
    ui->table->setItem(row, 1, new QTableWidgetItem(value));
}

void RunConfigDialog::upsertEntry()
{
    const QString key = ui->keyEdit->text().trimmed();
    const QString value = ui->valueEdit->text();

    if (key.isEmpty())
        return;
    if (key.contains(QLatin1Char('='))) {
        QMessageBox::warning(this, tr("Invalid key"),
                             tr("Environment variable names cannot contain '='."));
        return;
    }

    int row = findRowByKey(key);
    if (row < 0) {
        row = ui->table->rowCount();
        ui->table->insertRow(row);
    }
    setRow(row, key, value);
	ApplicationSettings settings;
	settings.beginGroup("EnvironmentVariable");
	settings.setValue(key, value);
	settings.endGroup();

    ui->table->clearSelection();
    ui->keyEdit->clear();
    ui->valueEdit->clear();
    ui->keyEdit->setFocus();
}

void RunConfigDialog::deleteEntry()
{
    const int row = findRowByKey(ui->keyEdit->text().trimmed());
    if (row < 0)
        return;
	ApplicationSettings settings;
	settings.beginGroup("EnvironmentVariable");
	settings.remove(ui->keyEdit->text().trimmed());
	settings.endGroup();
    ui->table->removeRow(row);
    ui->keyEdit->clear();
    ui->valueEdit->clear();
}

void RunConfigDialog::loadRowIntoInputs()
{
    const auto selected = ui->table->selectedItems();
    if (selected.isEmpty())
        return;
    const int row = selected.first()->row();
    ui->keyEdit->setText(ui->table->item(row, 0)->text());
    ui->valueEdit->setText(ui->table->item(row, 1)->text());
}

// Replaces the first "<filename>" token if present (in the text, or in the
// default command when the field is empty); otherwise inserts at the cursor.
void RunConfigDialog::insertIntoCommand(const QString &path)
{
    QString text = ui->commandEdit->text();
    if (text.isEmpty())
        text = QString::fromLatin1(kDefaultCommand);

    const int idx = text.indexOf(QLatin1String(kFilenameToken));
    if (idx >= 0) {
        text.replace(idx, int(qstrlen(kFilenameToken)), path);
        ui->commandEdit->setText(text);
    } else {
        ui->commandEdit->insert(path);
    }
}

// ---------------------------------------------------------------------------
// Persistence
// ---------------------------------------------------------------------------

void RunConfigDialog::loadConfig()
{
    ApplicationSettings settings;
	
	settings.beginGroup("FolderAsWorkspace");
	QString RootPath = settings.value("RootPath").toString();
	settings.endGroup();
	
	settings.beginGroup("EnvironmentVariable");
	settings.setValue("WorkingDirectory", RootPath);
	
	QStringList EnvironmentVariableList = settings.childKeys();
	int rowIndex = 0;
	ui->table->setRowCount(EnvironmentVariableList.size());
	for (const QString &EnvironmentVariable : EnvironmentVariableList)
	{
		setRow(rowIndex++, EnvironmentVariable, settings.value(EnvironmentVariable).toString());
	}
    settings.endGroup();
	
	settings.beginGroup("RunConfigurations");
	
	QStringList runConfigurationList = settings.childKeys();
	filteredConfigList.clear();
	
	for(const QString &runConfiguration: runConfigurationList)
	{
		qInfo("Detected rootDir: %s\nrun-conf: %s", qUtf8Printable(RootPath), qUtf8Printable(runConfiguration));
		QStringList tmpConfig = runConfiguration.split("#");
		if(tmpConfig[0]==RunConfigDialog::encode(QDir::cleanPath(RootPath)))
		{
			filteredConfigList.append(tmpConfig[1]);
		}
	}
	runConfigNamesByWorkingDirectory = new QCompleter(filteredConfigList,ui->buildName);
	runConfigNamesByWorkingDirectory->setCompletionMode(QCompleter::PopupCompletion);
	runConfigNamesByWorkingDirectory->setFilterMode(Qt::MatchContains);
	runConfigNamesByWorkingDirectory->setCaseSensitivity(Qt::CaseInsensitive);
	
	ui->buildName->setCompleter(runConfigNamesByWorkingDirectory);
	
	connect(runConfigNamesByWorkingDirectory, QOverload<const QString &>::of(&QCompleter::activated), this, &RunConfigDialog::fillBuildDetails);
	
	settings.endGroup();
}

void RunConfigDialog::fillBuildDetails(QString buildNameFromCompleter)
{
	ApplicationSettings settings;
	
	settings.beginGroup("FolderAsWorkspace");
	QString RootPath = settings.value("RootPath").toString();
	settings.endGroup();
	
	settings.beginGroup("RunConfigurations");
	ui->buildName->setText(buildNameFromCompleter);
	ui->commandEdit->setText(settings.value(RunConfigDialog::encode(QDir::cleanPath(RootPath))+"#"+buildNameFromCompleter).toString());
	settings.endGroup();
}

QString RunConfigDialog::encode(const QString &path)
{
	return QString::fromLatin1(path.toUtf8().toPercentEncoding());
}

void RunConfigDialog::onSave()
{
    ApplicationSettings settings;
	settings.beginGroup("EnvironmentVariable");
	QString workingDirectory = settings.value("WorkingDirectory").toString();
	settings.endGroup();
	
	settings.beginGroup("RunConfigurations");
	settings.setValue((RunConfigDialog::encode(QDir::cleanPath(workingDirectory))+"#"+(ui->buildName->text())),ui->commandEdit->text());
	
    settings.sync();
	settings.endGroup();
	if (!filteredConfigList.contains(ui->buildName->text())) filteredConfigList.append(ui->buildName->text());
	runConfigNamesByWorkingDirectory = new QCompleter(filteredConfigList,ui->buildName);
	runConfigNamesByWorkingDirectory->setCompletionMode(QCompleter::PopupCompletion);
	runConfigNamesByWorkingDirectory->setFilterMode(Qt::MatchContains);
	runConfigNamesByWorkingDirectory->setCaseSensitivity(Qt::CaseInsensitive);
	
	ui->buildName->setCompleter(runConfigNamesByWorkingDirectory);
	connect(runConfigNamesByWorkingDirectory, QOverload<const QString &>::of(&QCompleter::activated), this, &RunConfigDialog::fillBuildDetails);
	
}

void RunConfigDialog::onDelete()
{
	ApplicationSettings settings;
	settings.beginGroup("EnvironmentVariable");
	QString workingDirectory = settings.value("WorkingDirectory").toString();
	settings.endGroup();
	
	settings.beginGroup("RunConfigurations");
	settings.remove(RunConfigDialog::encode(QDir::cleanPath(workingDirectory))+"#"+ui->buildName->text());
	settings.endGroup();
	if(filteredConfigList.removeOne(ui->buildName->text()))
	{
	runConfigNamesByWorkingDirectory = new QCompleter(filteredConfigList,ui->buildName);
	runConfigNamesByWorkingDirectory->setCompletionMode(QCompleter::PopupCompletion);
	runConfigNamesByWorkingDirectory->setFilterMode(Qt::MatchContains);
	runConfigNamesByWorkingDirectory->setCaseSensitivity(Qt::CaseInsensitive);
	
	ui->buildName->setCompleter(runConfigNamesByWorkingDirectory);
	connect(runConfigNamesByWorkingDirectory, QOverload<const QString &>::of(&QCompleter::activated), this, &RunConfigDialog::fillBuildDetails);
	}
	ui->buildName->setText("");
	ui->commandEdit->setText("");
}

// ---------------------------------------------------------------------------
// Run: execute and close
// ---------------------------------------------------------------------------

void RunConfigDialog::onRun()
{
    const QString cmd = ui->commandEdit->text().trimmed();
    if (cmd.isEmpty()) {
        QMessageBox::warning(this, tr("No command"), tr("The command field is empty."));
        return;
    }
    if (cmd.contains(QLatin1String(kFilenameToken))) {
        QMessageBox::warning(this, tr("Unresolved token"),
                             tr("The command still contains <filename>. "
                                "Use the file / folder selection button."));
        return;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for (int i = 0; i < ui->table->rowCount(); ++i)
        env.insert(ui->table->item(i, 0)->text(), ui->table->item(i, 1)->text());

    // A shell is required because the command may contain pipes.
    QProcess process;
#ifdef Q_OS_WIN
    process.setProgram(QStringLiteral("cmd"));
    process.setArguments({QStringLiteral("/c"), cmd});
#else
    process.setProgram(QStringLiteral("/bin/sh"));
    process.setArguments({QStringLiteral("-c"), cmd});
#endif
    process.setProcessEnvironment(env);

    if (!process.startDetached()) {
        QMessageBox::critical(this, tr("Run failed"), process.errorString());
        return;
    }
    accept();
}
