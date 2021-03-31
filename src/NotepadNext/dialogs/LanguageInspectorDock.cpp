#include "LanguageInspectorDock.h"
#include "ui_LanguageInspectorDock.h"

#include "MainWindow.h"
#include "LanguagePropertiesModel.h"

#include <QCheckBox>

QTableWidgetItem *item_string(const QString &name, bool edit = false) {
    QTableWidgetItem *item = new QTableWidgetItem(name);
    item->setToolTip(name);

    if (!edit)
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    return item;
}

QTableWidgetItem *item_int(int value, bool edit = false) {
    QTableWidgetItem *item = item_string(QString::number(value), edit);
    item->setTextAlignment(Qt::AlignCenter);

    return item;
}

QWidget *item_bool(bool checked) {
    QWidget *pWidget = new QWidget();
    QCheckBox *pCheckBox = new QCheckBox();
    QHBoxLayout *pLayout = new QHBoxLayout(pWidget);

    pCheckBox->setChecked(checked);
    pLayout->addWidget(pCheckBox);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0,0,0,0);
    pWidget->setLayout(pLayout);

    return pWidget;
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

    connect(ui->tblStyles, &QTableWidget::cellChanged, [=](int row, int column) {
        if (column == 5) {
            int id = ui->tblStyles->item(row, 0)->text().toInt();
            int size = ui->tblStyles->item(row, column)->text().toInt();

            auto editor = dockedEditor->getCurrentEditor();

            editor->styleSetSize(id, size);

            editor->colourise(0, -1);
        }
        else if (column == 7) {
            int id = ui->tblStyles->item(row, 0)->text().toInt();
            bool bold = qobject_cast<QCheckBox *>(ui->tblStyles->cellWidget(row, column))->isChecked();

            auto editor = dockedEditor->getCurrentEditor();

            editor->styleSetBold(id, bold);

            editor->colourise(0, -1);
        }
    });
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

    ui->tblProperties->model()->deleteLater();
    ui->tblProperties->setModel(new LanguagePropertiesModel(editor));

    ui->tblProperties->resizeColumnToContents(0);
    ui->tblProperties->resizeColumnToContents(1);
    ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tblProperties->resizeColumnToContents(3);


    this->updateKeywordInfo(editor);
    this->updateStyleInfo(editor);
}

void LanguageInspectorDock::updateLanguageName(ScintillaNext *editor)
{
    ui->editLanguage->setText(editor->property("nn.meta.language"));
    ui->editLexer->setText(editor->lexerLanguage());
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
            ui->tblKeywords->setItem(row, 0, item_int(row));
            ui->tblKeywords->setItem(row, 1, item_string(keyword));

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
        ui->tblStyles->setItem(i, 0, item_int(i));
        ui->tblStyles->setItem(i, 1, item_string(editor->nameOfStyle(i)));
        ui->tblStyles->setItem(i, 2, item_string(editor->tagsOfStyle(i)));
        ui->tblStyles->setItem(i, 3, item_string(editor->descriptionOfStyle(i)));
        ui->tblStyles->setItem(i, 4, item_string(editor->styleFont(i)));
        ui->tblStyles->setItem(i, 5, item_int(editor->styleSize(i), true));
        ui->tblStyles->setItem(i, 6, item_int(editor->styleSizeFractional(i)));
        ui->tblStyles->setCellWidget(i, 7, item_bool(editor->styleBold(i)));
        ui->tblStyles->setItem(i, 8, item_int(editor->styleWeight(i)));
        ui->tblStyles->setCellWidget(i, 9, item_bool(editor->styleItalic(i)));
        ui->tblStyles->setCellWidget(i, 10, item_bool(editor->styleUnderline(i)));
        ui->tblStyles->setItem(i, 11, item_int(editor->styleFore(i)));
        ui->tblStyles->setItem(i, 12, item_int(editor->styleBack(i)));
        ui->tblStyles->setCellWidget(i, 13, item_bool(editor->styleEOLFilled(i)));
        ui->tblStyles->setItem(i, 14, item_int(editor->styleCharacterSet(i)));
        ui->tblStyles->setItem(i, 15, item_int(editor->styleCase(i)));
        ui->tblStyles->setCellWidget(i, 16, item_bool(editor->styleVisible(i)));
        ui->tblStyles->setCellWidget(i, 17, item_bool(editor->styleChangeable(i)));
        ui->tblStyles->setCellWidget(i, 18, item_bool(editor->styleHotSpot(i)));
    }

    ui->tblStyles->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(8, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(10, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(11, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(12, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(13, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(14, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(15, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(16, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(17, QHeaderView::ResizeToContents);
    ui->tblStyles->horizontalHeader()->setSectionResizeMode(18, QHeaderView::ResizeToContents);
}
