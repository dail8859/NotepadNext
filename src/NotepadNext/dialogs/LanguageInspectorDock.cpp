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

QTableWidgetItem *default_widget_item(const QString &name) {
    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setToolTip(name);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    return item;
}

LanguageInspectorDock::LanguageInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::LanguageInspectorDock)
{
    ui->setupUi(this);

    DockedEditor *dockedEditor = parent->getDockedEditor();

    connect(dockedEditor, &DockedEditor::editorActivated, this, &LanguageInspectorDock::updateInformation);
    connect(this, &LanguageInspectorDock::visibilityChanged, [=](bool visible) {
        if (visible) {
            MainWindow *mw = qobject_cast<MainWindow *>(this->parent());
            updateInformation(mw->currentEditor());
        }
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

void LanguageInspectorDock::updateInformation(ScintillaNext *editor)
{
    // Don't update if the dock widget is not visible
    if (this->isHidden()) return;

    this->updateLanguageName(editor);
    this->updatePropertyInfo(editor);
    this->updateKeywordInfo(editor);
    this->updateStyleInfo(editor);
}

void LanguageInspectorDock::updateLanguageName(ScintillaNext *editor)
{
    ui->editLanguage->setText(editor->property("nn.meta.language"));
    ui->editLexer->setText(editor->lexerLanguage());
}

void LanguageInspectorDock::updatePropertyInfo(ScintillaNext *editor)
{
    const QString propertyNames = QString(editor->propertyNames());
    const QSignalBlocker blocker(ui->tblProperties);

    ui->tblProperties->clearContents();

    if (!propertyNames.isEmpty()) {
        const QStringList properties = propertyNames.split('\n');
        ui->tblProperties->setRowCount(properties.size());

        int row = 0;
        foreach (const QString &property, properties) {
            ui->tblProperties->setItem(row, 0, default_widget_item(property));
            ui->tblProperties->setItem(row, 1, default_widget_item(property_type_to_string(editor->propertyType(property.toLatin1().constData()))));
            ui->tblProperties->setItem(row, 2, default_widget_item(editor->describeProperty(property.toLatin1().constData())));
            ui->tblProperties->setItem(row, 3, default_widget_item(editor->property(property.toLatin1().constData())));

            row++;
        }

        ui->tblProperties->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        ui->tblProperties->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    }
    else {
        ui->tblProperties->setRowCount(0);
    }
}

void LanguageInspectorDock::updateKeywordInfo(ScintillaNext *editor)
{
    const QString keyWordSets = QString(editor->describeKeyWordSets());
    const QSignalBlocker blocker(ui->tblKeywords);

    ui->tblKeywords->clearContents();

    if (!keyWordSets.isEmpty()) {
        const QStringList keywords = keyWordSets.split('\n');

        ui->tblKeywords->setRowCount(keywords.size());

        int row = 0;
        foreach (const QString &keyword, keywords) {
            ui->tblKeywords->setItem(row, 0, default_widget_item(QString::number(row)));
            ui->tblKeywords->setItem(row, 1, default_widget_item(keyword));

            row++;
        }

        ui->tblKeywords->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        ui->tblKeywords->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    }
    else {
        ui->tblKeywords->setRowCount(0);
    }
}

void LanguageInspectorDock::updateStyleInfo(ScintillaNext *editor)
{
    const int num_styles = editor->namedStyles();
    const QSignalBlocker blocker(ui->tblStyles);

    ui->tblStyles->clearContents();
    ui->tblStyles->setRowCount(num_styles);

    for(int i = 0; i < num_styles; i++) {
        ui->tblStyles->setItem(i, 0, default_widget_item(QString::number(i)));
        ui->tblStyles->setItem(i, 1, default_widget_item(editor->nameOfStyle(i)));
        ui->tblStyles->setItem(i, 2, default_widget_item(editor->tagsOfStyle(i)));
        ui->tblStyles->setItem(i, 3, default_widget_item(editor->descriptionOfStyle(i)));
    }

    ui->tblStyles->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
}
