// ListDialog.cpp
// started 20220816 afo@fly130

#include "ListDialog.h"
#include "ui_ListDialog.h"

#include <QListWidgetItem>

ListDialog::ListDialog(QWidget *parent)
	: QDialog(parent, Qt::Tool)
	, ui(new Ui::ListDialog)
{
    ui->setupUi(this);

	connect(ui->okButton, &QPushButton::clicked, this, [=]() {
			m_currentLine = ui->listWidget->currentItem()->text();
			QDialog::done(QDialog::Accepted);
	});

	connect(ui->applyButton, &QPushButton::clicked, this, [=]() {
		auto item = ui->listWidget->takeItem(ui->listWidget->currentRow());
		if(nullptr != item)
		{
			emit clickedApply(item->text());
			delete item;
			ui->listWidget->setFocus();
		}
	});
}

ListDialog::~ListDialog()
{
    delete ui;
}

void ListDialog::clear()
{
	ui->listWidget->clear();
}

int ListDialog::count() const
{
	return ui->listWidget->count();
}


void ListDialog::addLine(const QString& str)
{
	ui->listWidget->addItem(str);
}

void ListDialog::addList(const QStringList& strList)
{
	for(auto const& str : strList) { ui->listWidget->addItem(str); }
}

void ListDialog::setList(const QStringList& strList)
{
	ui->listWidget->clear();
	addList(strList);
}

QString ListDialog::getCurrentLine() const
{
	return m_currentLine != "" ? m_currentLine : ui->listWidget->currentItem()->text();
}

void ListDialog::setCurrentLine(const QString& str)
{
	auto listW(ui->listWidget);
	for(int row = 0; row < listW->count(); ++row)
	{
		if(str == listW->item(row)->text())
		{
			listW->setCurrentRow(row);
			return;
		}
	}
}

void ListDialog::setButtonLabels(const QString& okLabel, const QString& applyLabel, const QString& cancelLabel)
{
	if(okLabel != "") { ui->okButton->setText(okLabel); }
	if(applyLabel != "") { ui->applyButton->setText(applyLabel); }
	if(cancelLabel != "") { ui->cancelButton->setText(cancelLabel); }
}

void ListDialog::slotItemActivated(QListWidgetItem*)
{
	m_currentLine = ui->listWidget->currentItem()->text();
}

void ListDialog::slotItemDoubleClicked(QListWidgetItem*)
{
	m_currentLine = ui->listWidget->currentItem()->text();
	QDialog::done(QDialog::Accepted);
}

int ListDialog::exec()
{
	show();
	raise();
	activateWindow();
	//ui->listWidget->setFocus();

    return QDialog::exec();
}
