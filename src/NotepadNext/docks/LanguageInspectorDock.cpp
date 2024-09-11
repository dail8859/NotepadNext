/*
 * This file is part of Notepad Next.
 * Copyright 2021 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QFontDatabase>

#include "LanguageInspectorDock.h"
#include "ui_LanguageInspectorDock.h"

#include "MainWindow.h"
#include "LanguageStylesModel.h"

#include "SpinBoxDelegate.h"
#include "ColorPickerDelegate.h"
#include "ComboBoxDelegate.h"

#include "ScintillaNext.h"

static QSpinBox *FontSizeSpinBoxFactory()
{
    QSpinBox *editor = new QSpinBox();
    editor->setFrame(false);
    editor->setRange(2, 24);
    editor->setAlignment(Qt::AlignHCenter);
    return editor;
}

static QSpinBox *FontWeightSpinBoxFactory()
{
    QSpinBox *editor = new QSpinBox();
    editor->setFrame(false);
    editor->setRange(100, 2400);
    editor->setSingleStep(10);
    editor->setAlignment(Qt::AlignHCenter);
    return editor;
}

static QString property_type_to_string(int type) {
    switch (type) {
    case SC_TYPE_BOOLEAN:
        return "Bool";
    case SC_TYPE_INTEGER:
        return "Int";
    case SC_TYPE_STRING:
        return "String";
    default:
        return "unknown";
    }
}

LanguageInspectorDock::LanguageInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::LanguageInspectorDock)
{
    ui->setupUi(this);

    SpinBoxDelegate *fontSizeDelegate = new SpinBoxDelegate(FontSizeSpinBoxFactory, this);
    ui->tblStyles->setItemDelegateForColumn(5, fontSizeDelegate);

    SpinBoxDelegate *fontWeightDelegate = new SpinBoxDelegate(FontWeightSpinBoxFactory, this);
    ui->tblStyles->setItemDelegateForColumn(6, fontWeightDelegate);

    ColorPickerDelegate *foreColorDelegate = new ColorPickerDelegate(this);
    ui->tblStyles->setItemDelegateForColumn(11, foreColorDelegate);

    ColorPickerDelegate *backColorDelegate = new ColorPickerDelegate(this);
    ui->tblStyles->setItemDelegateForColumn(12, backColorDelegate);

    QList<ComboBoxItem> caseItems{
        {"SC_CASE_MIXED", SC_CASE_MIXED},
        {"SC_CASE_UPPER", SC_CASE_UPPER},
        {"SC_CASE_LOWER", SC_CASE_LOWER},
        {"SC_CASE_CAMEL", SC_CASE_CAMEL}
    };
    ComboBoxDelegate *caseComoboDelegate = new ComboBoxDelegate(caseItems, this);
    ui->tblStyles->setItemDelegateForColumn(15, caseComoboDelegate);

    QList<ComboBoxItem> fontNames;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    for (const QString &font : QFontDatabase::families()) {
#else
    QFontDatabase fontDatabase;
    for (const QString &font : fontDatabase.families()) {
#endif
        fontNames.append(QPair<QString, QString>(font, font));
    }
    ComboBoxDelegate *fontComboDelegate = new ComboBoxDelegate(fontNames, this);
    ui->tblStyles->setItemDelegateForColumn(4, fontComboDelegate);

    connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            connectToEditor(parent->currentEditor());
            connect(parent, &MainWindow::editorActivated, this, &LanguageInspectorDock::connectToEditor);
        }
        else {
            disconnectFromEditor();
            disconnect(parent, &MainWindow::editorActivated, this, &LanguageInspectorDock::connectToEditor);
        }
    });
}

LanguageInspectorDock::~LanguageInspectorDock()
{
    delete ui;
}

void LanguageInspectorDock::connectToEditor(ScintillaNext *editor)
{
    disconnectFromEditor();

    editorConnection = connect(editor, &ScintillaNext::updateUi, this, &LanguageInspectorDock::updatePositionInfo);
    documentConnection = connect(editor, &ScintillaNext::lexerChanged, this, [=]() { updateLexerInfo(editor); });

    updateLexerInfo(editor);
}

void LanguageInspectorDock::disconnectFromEditor()
{
    if (editorConnection) {
        disconnect(editorConnection);
    }

    if (documentConnection) {
        disconnect(documentConnection);
    }
}

void LanguageInspectorDock::updatePositionInfo(Scintilla::Update updated)
{
    if (FlagSet(updated, Scintilla::Update::Content) || FlagSet(updated, Scintilla::Update::Selection)) {
        ScintillaNext *editor = qobject_cast<ScintillaNext*>(sender());
        ui->lblInfo->setText(tr("Position %1 Style %2").arg(editor->currentPos()).arg(editor->styleAt(editor->currentPos())));
    }
}

void LanguageInspectorDock::updateLexerInfo(ScintillaNext *editor)
{
    updateLanguageName(editor);
    updatePropertyInfo(editor);
    updateKeywordInfo(editor);
    updateStyleInfo(editor);
}

void LanguageInspectorDock::updateLanguageName(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    ui->editLanguage->setText(editor->languageName);
    ui->editLexer->setText(editor->lexerLanguage());
}

void LanguageInspectorDock::updatePropertyInfo(ScintillaNext *editor)
{
    const QSignalBlocker blocker(ui->tblProperties);

    ui->tblProperties->clearContents();

    const QString propertyNames = editor->propertyNames();

    if (propertyNames.isEmpty()) {
        ui->tblProperties->setRowCount(0);
    }
    else {
        QStringList propertyNamesList = propertyNames.split('\n');

        ui->tblProperties->setRowCount(propertyNamesList.count());

        for (int i = 0; i < propertyNamesList.count(); ++i) {
            const auto property = propertyNamesList[i];

            QTableWidgetItem *prop = new QTableWidgetItem(property);
            QTableWidgetItem *type = new QTableWidgetItem(property_type_to_string(editor->propertyType(property.toLatin1().constData())));
            QTableWidgetItem *desc = new QTableWidgetItem(QString(editor->describeProperty(property.toLatin1().constData())));
            QTableWidgetItem *val = new QTableWidgetItem(QString(editor->property(property.toLatin1().constData())));

            desc->setToolTip(desc->text());

            prop->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            type->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            desc->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            val->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

            val->setTextAlignment(Qt::AlignCenter);

            ui->tblProperties->setItem(i, 0, prop);
            ui->tblProperties->setItem(i, 1, type);
            ui->tblProperties->setItem(i, 2, desc);
            ui->tblProperties->setItem(i, 3, val);
        }
    }

    ui->tblProperties->resizeColumnToContents(0);
    ui->tblProperties->resizeColumnToContents(1);
    ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tblProperties->resizeColumnToContents(3);

    ui->tblProperties->disconnect();
    connect(ui->tblProperties, &QTableWidget::itemChanged, this, [=](QTableWidgetItem *item) {
        const QString property = ui->tblProperties->item(item->row(), 0)->text();

        editor->setProperty(property.toLatin1().constData(), item->text().toLatin1().constData());

        editor->colourise(0, -1);
    });
}

void LanguageInspectorDock::updateKeywordInfo(ScintillaNext *editor)
{
    ui->tblKeywords->clearContents();

    const QString keyWordSetsDescription = QString(editor->describeKeyWordSets());

    if (keyWordSetsDescription.isEmpty()) {
        ui->tblKeywords->setRowCount(0);
    }
    else {
        QStringList keyWordsSets = keyWordSetsDescription.split('\n');

        ui->tblKeywords->setRowCount(keyWordsSets.count());

        for (int i = 0; i < keyWordsSets.count(); ++i) {
            auto id = new QTableWidgetItem(QString::number(i));
            id->setTextAlignment(Qt::AlignCenter);

            ui->tblKeywords->setItem(i, 0, id);
            ui->tblKeywords->setItem(i, 1, new QTableWidgetItem(keyWordsSets[i]));
        }
    }

    ui->tblKeywords->resizeColumnToContents(0);
}

void LanguageInspectorDock::updateStyleInfo(ScintillaNext *editor)
{
    if (auto model = ui->tblStyles->model()) {
        model->deleteLater();
    }
    ui->tblStyles->setModel(new LanguageStylesModel(editor));

    ui->tblStyles->resizeColumnToContents(0);
    ui->tblStyles->resizeColumnToContents(1);
    ui->tblStyles->resizeColumnToContents(5);
    ui->tblStyles->resizeColumnToContents(6);
    ui->tblStyles->resizeColumnToContents(7);
    ui->tblStyles->resizeColumnToContents(8);
    ui->tblStyles->resizeColumnToContents(9);
    ui->tblStyles->resizeColumnToContents(10);
    ui->tblStyles->resizeColumnToContents(11);
    ui->tblStyles->resizeColumnToContents(12);
    ui->tblStyles->resizeColumnToContents(13);
    ui->tblStyles->resizeColumnToContents(14);
    ui->tblStyles->resizeColumnToContents(15);
    ui->tblStyles->resizeColumnToContents(16);
    ui->tblStyles->resizeColumnToContents(17);
    ui->tblStyles->resizeColumnToContents(18);
}
