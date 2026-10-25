#include "RunConfigDialog.h"
#include "ui_RunConfigDialog.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>


#include "MainWindow.h"

namespace {
const char kFilenameToken[] = "<filename>";
const char kDefaultCommand[] = "go run <filename>";
}

RunConfigDialog::RunConfigDialog(QSettings *runConf, MainWindow *parent)
    : QDialog(parent, Qt::Dialog)
    , ui(new Ui::RunConfigDialog),
	runConfSettings(runConf)
{
    ui->setupUi(this);

    // File / folder selection menu (menus are attached in code, not in the .ui)
    auto *pathMenu = new QMenu(ui->pathButton);
    pathMenu->addAction(tr("Select file..."), this, &RunConfigDialog::selectFile);
    pathMenu->addAction(tr("Select folder..."), this, &RunConfigDialog::selectFolder);
    ui->pathButton->setMenu(pathMenu);

    connect(ui->upsertButton, &QPushButton::clicked, this, &RunConfigDialog::upsertEntry);
    connect(ui->deleteButton, &QPushButton::clicked, this, &RunConfigDialog::deleteEntry);
    connect(ui->valueEdit, &QLineEdit::returnPressed, this, &RunConfigDialog::upsertEntry);
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &RunConfigDialog::loadRowIntoInputs);
    connect(ui->runButton, &QPushButton::clicked, this, &RunConfigDialog::onRun);
    connect(ui->saveButton, &QPushButton::clicked, this, &RunConfigDialog::onSave);
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

// ---------------------------------------------------------------------------
// File / folder selection
// ---------------------------------------------------------------------------

void RunConfigDialog::selectFile()
{
    const QString path = QFileDialog::getOpenFileName(this, tr("Select file"));
    if (!path.isEmpty())
        insertIntoCommand(path);
}

void RunConfigDialog::selectFolder()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Select folder"));
    if (!path.isEmpty())
        insertIntoCommand(path);
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
    QSettings s;
    ui->commandEdit->setText(s.value(QStringLiteral("command")).toString());

    const int n = s.beginReadArray(QStringLiteral("env"));
    for (int i = 0; i < n; ++i) {
        s.setArrayIndex(i);
        const int row = ui->table->rowCount();
        ui->table->insertRow(row);
        setRow(row, s.value(QStringLiteral("key")).toString(),
               s.value(QStringLiteral("value")).toString());
    }
    s.endArray();
}

void RunConfigDialog::onSave()
{
    QSettings s;
    s.setValue(QStringLiteral("command"), ui->commandEdit->text());

    s.beginWriteArray(QStringLiteral("env"), ui->table->rowCount());
    for (int i = 0; i < ui->table->rowCount(); ++i) {
        s.setArrayIndex(i);
        s.setValue(QStringLiteral("key"), ui->table->item(i, 0)->text());
        s.setValue(QStringLiteral("value"), ui->table->item(i, 1)->text());
    }
    s.endArray();
    s.sync();
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
