#include "LanguageInspectorDock.h"
#include "ui_LanguageInspectorDock.h"

#include "MainWindow.h"

QString property_type_to_string(int type) {
    switch (type) {
    case SC_TYPE_BOOLEAN:
        return "bool";
    case SC_TYPE_INTEGER:
        return "integer";
    case SC_TYPE_STRING:
        return "string";
    default:
        return "unknown";
    }
}

LanguageInspectorDock::LanguageInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::LanguageInspectorDock)
{
    ui->setupUi(this);
    dockedEditor = parent->getDockedEditor();


    connect(dockedEditor, &DockedEditor::editorActivated, [=](ScintillaNext *editor) {
        ui->editLanguage->setText(editor->property("nn.meta.language"));
        ui->editLexer->setText(editor->lexerLanguage());

        QSignalBlocker(ui->tblProperties);
        QSignalBlocker(ui->tblKeywords);

        ui->tblProperties->clear();
        ui->tblKeywords->clear();

        QStringList properties = QString(editor->propertyNames()).split('\n');
        ui->tblProperties->setColumnCount(4);
        ui->tblProperties->setRowCount(properties.size());

        int row = 0;
        foreach (const QString &property, properties) {
            QTableWidgetItem *item;

            item = new QTableWidgetItem(property);
            item->setToolTip(property);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tblProperties->setItem(row, 0, item);

            item = new QTableWidgetItem(property_type_to_string(editor->propertyType(property.toLatin1().constData())));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tblProperties->setItem(row, 1, item);

            item = new QTableWidgetItem(QString(editor->describeProperty(property.toLatin1().constData())));
            item->setToolTip(QString(editor->describeProperty(property.toLatin1().constData())));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tblProperties->setItem(row, 2, item);

            item = new QTableWidgetItem(QString(editor->property(property.toLatin1().constData())));
            ui->tblProperties->setItem(row, 3, item);

            row++;
        }

        ui->tblProperties->setHorizontalHeaderLabels(QStringList() << "Property" << "Type" << "Description" << "Value");
        ui->tblProperties->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tblProperties->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);


        QStringList keywords = QString(editor->describeKeyWordSets()).split('\n');
        ui->tblKeywords->setColumnCount(2);
        ui->tblKeywords->setRowCount(keywords.size());

        row = 0;
        foreach (const QString &keyword, keywords) {
            QTableWidgetItem *item;

            item = new QTableWidgetItem(QString::number(row));
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tblKeywords->setItem(row, 0, item);

            item = new QTableWidgetItem(keyword);
            item->setToolTip(keyword);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui->tblKeywords->setItem(row, 1, item);

            row++;
        }

        ui->tblKeywords->setHorizontalHeaderLabels(QStringList() << "Index" << "Description");
        ui->tblKeywords->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        ui->tblKeywords->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    });

    //connect(ui->tblProperties, &QTableWidget::cellChanged, [=](int row, int column) {
    //    if (column != 3) return;
    //
    //    QString property = ui->tableWidget->item(row, 0)->text();
    //    QString value = ui->tableWidget->item(row, column)->text();
    //    auto editor = dockedEditor->getCurrentEditor();
    //
    //    editor->setProperty(property.toLatin1().constData(), value.toLatin1().constData());
    //
    //    editor->colourise(0, -1);
    //});
}

LanguageInspectorDock::~LanguageInspectorDock()
{
    delete ui;
}
