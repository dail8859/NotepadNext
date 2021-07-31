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
#include "LanguagePropertiesModel.h"
#include "LanguageKeywordsModel.h"
#include "LanguageStylesModel.h"

#include "SpinBoxDelegate.h"
#include "ColorPickerDelegate.h"
#include "ComboBoxDelegate.h"

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

LanguageInspectorDock::LanguageInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(new Ui::LanguageInspectorDock)
{
    ui->setupUi(this);

    DockedEditor *dockedEditor = parent->getDockedEditor();

    connect(dockedEditor, &DockedEditor::editorActivated, this, &LanguageInspectorDock::updateInformation);
    connect(dockedEditor, &DockedEditor::editorAdded, [=](ScintillaNext *editor) {
        connect(editor->getBuffer(), &ScintillaBuffer::lexer_changed, [=](){
            updateInformation(editor);
        });
    });
    connect(this, &LanguageInspectorDock::visibilityChanged, [=](bool visible) {
        if (visible) {
            MainWindow *mw = qobject_cast<MainWindow *>(this->parent());
            updateInformation(mw->currentEditor());
        }
    });

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

    QFontDatabase fontDatabase;
    QList<ComboBoxItem> fontNames;
    foreach(const QString font, fontDatabase.families()) {
        fontNames.append(QPair(font, font));
    }
    ComboBoxDelegate *fontComboDelegate = new ComboBoxDelegate(fontNames, this);
    ui->tblStyles->setItemDelegateForColumn(4, fontComboDelegate);
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
    ui->editLanguage->setText(editor->languageName);
    ui->editLexer->setText(editor->lexerLanguage());
}

void LanguageInspectorDock::updatePropertyInfo(ScintillaNext *editor)
{
    ui->tblProperties->model()->deleteLater();
    ui->tblProperties->setModel(new LanguagePropertiesModel(editor));

    ui->tblProperties->resizeColumnToContents(0);
    ui->tblProperties->resizeColumnToContents(1);
    ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tblProperties->resizeColumnToContents(3);
}

void LanguageInspectorDock::updateKeywordInfo(ScintillaNext *editor)
{
    ui->tblKeywords->model()->deleteLater();
    ui->tblKeywords->setModel(new LanguageKeywordsModel(editor));

    ui->tblKeywords->resizeColumnToContents(0);
    ui->tblKeywords->horizontalHeader()->setStretchLastSection(true);
}

void LanguageInspectorDock::updateStyleInfo(ScintillaNext *editor)
{
    ui->tblStyles->model()->deleteLater();
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
