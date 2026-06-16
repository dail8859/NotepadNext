/*
 * This file is part of Notepad Next.
 * Copyright 2022 Justin Dailey
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


#include "ApplicationSettings.h"
#include "NotepadNextApplication.h"
#include "SearchResultHighlighterDelegate.h"
#include "SearchResultData.h"
#include "SearchResultsDock.h"
#include "ScintillaNext.h"
#include "ui_SearchResultsDock.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QShortcut>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>

#include <functional>
#include <utility>


namespace {

const QColor ResultRowBackground(220, 220, 220);

class SearchResultTextEdit : public QTextEdit
{
public:
    using ActivationHandler = std::function<void()>;
    using SelectionClearHandler = std::function<void()>;

    explicit SearchResultTextEdit(QWidget *parent = nullptr) :
        QTextEdit(parent)
    {
        setReadOnly(true);
        setAcceptRichText(false);
        setFrameShape(QFrame::NoFrame);
        setLineWrapMode(QTextEdit::NoWrap);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        setViewportMargins(4, 1, 4, 1);
        document()->setDocumentMargin(0);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setProperty("searchResultTextEdit", true);
    }

    void setActivationHandler(ActivationHandler handler)
    {
        activationHandler = std::move(handler);
    }

    void setSelectionClearHandler(SelectionClearHandler handler)
    {
        selectionClearHandler = std::move(handler);
    }

    QSize sizeHint() const override
    {
        const QFontMetrics fm(font());
        const int width = static_cast<int>(document()->idealWidth()) + 10;
        const int height = fm.height() + 4;
        return {width, height};
    }

    QSize minimumSizeHint() const override
    {
        return {0, sizeHint().height()};
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && selectionClearHandler) {
            selectionClearHandler();
        }

        QTextEdit::mousePressEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->matches(QKeySequence::Copy) && textCursor().hasSelection()) {
            QString text = textCursor().selectedText();
            text.replace(QChar::ParagraphSeparator, '\n');
            text.replace(QChar::LineSeparator, '\n');
            QGuiApplication::clipboard()->setText(text);
            event->accept();
            return;
        }

        QTextEdit::keyPressEvent(event);
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        QTextEdit::mouseDoubleClickEvent(event);

        if (activationHandler) {
            activationHandler();
        }
    }

private:
    ActivationHandler activationHandler;
    SelectionClearHandler selectionClearHandler;
};

QString selectedPlainText(QTextEdit *textEdit)
{
    QString text = textEdit->textCursor().selectedText();
    text.replace(QChar::ParagraphSeparator, '\n');
    text.replace(QChar::LineSeparator, '\n');
    return text;
}

bool copySelectedPlainText(QTextEdit *textEdit)
{
    if (textEdit == Q_NULLPTR || !textEdit->textCursor().hasSelection()) {
        return false;
    }

    QGuiApplication::clipboard()->setText(selectedPlainText(textEdit));
    return true;
}

bool isSearchResultTextEdit(QTextEdit *textEdit)
{
    return textEdit != Q_NULLPTR && textEdit->property("searchResultTextEdit").toBool();
}

void applyResultPalette(QTextEdit *textEdit)
{
    QPalette pal = textEdit->palette();
    pal.setColor(QPalette::Base, ResultRowBackground);
    pal.setColor(QPalette::Window, ResultRowBackground);
    textEdit->setPalette(pal);
    textEdit->viewport()->setPalette(pal);
}

void setHighlightedResultText(QTextEdit *textEdit, const QString &line, int startPositionFromBeginning, int endPositionFromBeginning)
{
    textEdit->setPlainText(line);

    const int start = qBound(0, startPositionFromBeginning, line.size());
    const int end = qBound(start, endPositionFromBeginning, line.size());
    QTextCursor cursor(textEdit->document());
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);
    highlightFormat.setForeground(Qt::red);
    highlightFormat.setFontWeight(QFont::Bold);
    cursor.mergeCharFormat(highlightFormat);

    cursor.clearSelection();
    cursor.movePosition(QTextCursor::Start);
    textEdit->setTextCursor(cursor);
}

}


SearchResultsDock::SearchResultsDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchResultsDock)
{
    ui->setupUi(this);
    ui->treeWidget->viewport()->installEventFilter(this);

    // Close the results when escape is pressed
    new QShortcut(QKeySequence::Cancel, this, this, &SearchResultsDock::close, Qt::WidgetWithChildrenShortcut);

    connect(ui->treeWidget, &QTreeWidget::itemActivated, this, &SearchResultsDock::itemActivated);
    connect(ui->treeWidget, &QTreeWidget::itemExpanded, this, &SearchResultsDock::itemExpanded);
    connect(ui->btnCopyResults, &QPushButton::released,this, &SearchResultsDock::copySearchResultsToClipboard);

    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);

        if (item == Q_NULLPTR) {
            return;
        }

        // Create the menu and show it
        QMenu menu(this);
        if (item->childCount() == 0 && item->parent() != Q_NULLPTR && item->data(1, SearchResultData::LineNumber).isValid()) {
            menu.addAction(tr("Copy Selected Line"), this, [item]() {
                QGuiApplication::clipboard()->setText(item->text(1));
            });
            menu.addSeparator();
        }
        menu.addAction(tr("Collapse All"), this, &SearchResultsDock::collapseAll);
        menu.addAction(tr("Expand All"), this, &SearchResultsDock::expandAll);
        menu.addSeparator();
        menu.addAction(tr("Delete Entry"), this, [this, item]() { deleteEntry(item); });
        menu.addSeparator();
        menu.addAction(tr("Delete All"), this, &SearchResultsDock::deleteAll);

        menu.exec(QCursor::pos());
    });

    ui->treeWidget->setUniformRowHeights(false);
    ui->treeWidget->setItemDelegate(new SearchResultHighlighterDelegate(ui->treeWidget));

    ApplicationSettings *settings = qobject_cast<NotepadNextApplication*>(qApp)->getSettings();
    auto updateTreeWidgetFont = [this, settings]() {
        QFont f(settings->fontName(), settings->fontSize());
        ui->treeWidget->setFont(f);
        for (QTextEdit *textEdit : ui->treeWidget->viewport()->findChildren<QTextEdit*>()) {
            if (textEdit->property("searchResultTextEdit").toBool()) {
                textEdit->setFont(f);
                textEdit->document()->adjustSize();
                textEdit->updateGeometry();
            }
        }
        ui->treeWidget->resizeColumnToContents(0);
        ui->treeWidget->resizeColumnToContents(1);
    };
    connect(settings, &ApplicationSettings::fontNameChanged, this, updateTreeWidgetFont);
    connect(settings, &ApplicationSettings::fontSizeChanged, this, updateTreeWidgetFont);
    updateTreeWidgetFont();
}

SearchResultsDock::~SearchResultsDock()
{
    delete ui;
}

bool SearchResultsDock::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->treeWidget->viewport() && event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            clearResultTextSelections();
        }
    }

    return QDockWidget::eventFilter(obj, event);
}

void SearchResultsDock::newSearch(const QString searchTerm)
{
    show();

    this->searchTerm = searchTerm;

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        const QTreeWidgetItem* topLevelItem = ui->treeWidget->topLevelItem(i);
        ui->treeWidget->collapseItem(topLevelItem);
    }

    currentSearch = new QTreeWidgetItem();
    ui->treeWidget->insertTopLevelItem(0, currentSearch);

    currentSearch->setBackground(0, QColor(232, 232, 255));
    currentSearch->setForeground(0, QColor(0, 0, 170));
    currentSearch->setExpanded(true);
    currentSearch->setFirstColumnSpanned(true);

    updateSearchStatus();
}

void SearchResultsDock::newFileEntry(ScintillaNext *editor)
{
    // Store a QPointer since there is no guarantee this editor will be around later
    QPointer<ScintillaNext> editor_pointer = editor;

    totalFileHitCount = 0;
    currentFilePath = editor->isFile() ? editor->getFilePath() : editor->getName();

    currentFile = new QTreeWidgetItem(currentSearch);
    currentFile->setData(0, Qt::UserRole, QVariant::fromValue(editor_pointer));

    currentFile->setBackground(0, QColor(213, 255, 213));
    currentFile->setForeground(0, QColor(0, 128, 0));
    currentFile->setExpanded(true);
    currentFile->setFirstColumnSpanned(true);

    currentFileCount++;
    updateSearchStatus();
}

void SearchResultsDock::newResultsEntry(const QString line, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning, int hitCount)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(currentFile);

    // Scintilla internally references line numbers starting at 0, however it needs displayed starting at 1
    item->setText(0, QString::number(lineNumber + 1));
    item->setBackground(0, QBrush(ResultRowBackground));
    item->setTextAlignment(0, Qt::AlignRight);

    item->setData(1, SearchResultData::LineNumber, lineNumber);
    item->setData(1, SearchResultData::LinePosStart, startPositionFromBeginning);
    item->setData(1, SearchResultData::LinePosEnd, endPositionFromBeginning);
    item->setText(1, line);

    auto *resultTextEdit = new SearchResultTextEdit(ui->treeWidget);
    resultTextEdit->setFont(ui->treeWidget->font());
    applyResultPalette(resultTextEdit);
    setHighlightedResultText(resultTextEdit, line, startPositionFromBeginning, endPositionFromBeginning);
    resultTextEdit->setActivationHandler([this, item]() {
        itemActivated(item, 1);
    });
    resultTextEdit->setSelectionClearHandler([this]() {
        clearResultTextSelections();
    });
    resultTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(resultTextEdit, &QWidget::customContextMenuRequested, this, [this, item, resultTextEdit](const QPoint &pos) {
        QMenu menu(this);

        QAction *copyAction = menu.addAction(tr("Copy"), this, [resultTextEdit]() {
            copySelectedPlainText(resultTextEdit);
        });
        copyAction->setEnabled(resultTextEdit->textCursor().hasSelection());
        menu.addAction(tr("Copy Selected Line"), this, [item]() {
            QGuiApplication::clipboard()->setText(item->text(1));
        });

        menu.addSeparator();
        menu.addAction(tr("Collapse All"), this, &SearchResultsDock::collapseAll);
        menu.addAction(tr("Expand All"), this, &SearchResultsDock::expandAll);
        menu.addSeparator();
        menu.addAction(tr("Delete Entry"), this, [this, item]() { deleteEntry(item); });
        menu.addSeparator();
        menu.addAction(tr("Delete All"), this, &SearchResultsDock::deleteAll);

        menu.exec(resultTextEdit->mapToGlobal(pos));
    });
    ui->treeWidget->setItemWidget(item, 1, resultTextEdit);

    totalFileHitCount += hitCount;
    totalHitCount += hitCount;

    updateSearchStatus();
}

void SearchResultsDock::completeSearch()
{
    currentSearch = Q_NULLPTR;
    currentFile = Q_NULLPTR;
    currentFileCount = 0;
    totalFileHitCount = 0;
    totalHitCount = 0;

    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
}

bool SearchResultsDock::copySelectedResultText()
{
    for (QWidget *widget = QApplication::focusWidget(); widget != Q_NULLPTR; widget = widget->parentWidget()) {
        auto *textEdit = qobject_cast<QTextEdit *>(widget);

        if (isSearchResultTextEdit(textEdit)) {
            return copySelectedPlainText(textEdit);
        }
    }

    if (QApplication::activePopupWidget() == Q_NULLPTR) {
        return false;
    }

    for (QTextEdit *textEdit : ui->treeWidget->findChildren<QTextEdit*>()) {
        if (isSearchResultTextEdit(textEdit) && copySelectedPlainText(textEdit)) {
            return true;
        }
    }

    return false;
}

void SearchResultsDock::clearResultTextSelections()
{
    for (QTextEdit *textEdit : ui->treeWidget->findChildren<QTextEdit*>()) {
        if (!isSearchResultTextEdit(textEdit)) {
            continue;
        }

        QTextCursor cursor = textEdit->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            textEdit->setTextCursor(cursor);
        }
    }
}

void SearchResultsDock::collapseAll() const
{
    ui->treeWidget->collapseAll();
}

void SearchResultsDock::expandAll() const
{
    ui->treeWidget->expandAll();
}

void SearchResultsDock::deleteEntry(QTreeWidgetItem *item)
{
    QTreeWidgetItem *parent = item->parent();

    if (parent != Q_NULLPTR) {
        parent->removeChild(item);
        delete item;
    }
    else {
        const int index = ui->treeWidget->indexOfTopLevelItem(item);
        delete ui->treeWidget->takeTopLevelItem(index);
    }
}

void SearchResultsDock::deleteAll()
{
    ui->treeWidget->clear();
}

void SearchResultsDock::itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    // Result entries have no children
    // Make sure the entry has a parent since search entries can have no children
    if (item->childCount() == 0 && item->parent() != Q_NULLPTR) {
        QPointer<ScintillaNext> editor = item->parent()->data(0, Qt::UserRole).value<QPointer<ScintillaNext>>();

        // The editor may no longer exist
        if (editor) {
            int lineNumber = item->data(1, SearchResultData::LineNumber).toInt();
            int startPositionFromBeginning = item->data(1, SearchResultData::LinePosStart).toInt();
            int endPositionFromBeginning = item->data(1, SearchResultData::LinePosEnd).toInt();

            emit searchResultActivated(editor, lineNumber, startPositionFromBeginning, endPositionFromBeginning);
        }
    }
}

void SearchResultsDock::itemExpanded(QTreeWidgetItem *)
{
    ui->treeWidget->resizeColumnToContents(1);
}

void SearchResultsDock::updateSearchStatus()
{
    currentSearch->setText(0, QStringLiteral("Search \"%1\" (%L2 hits in %L3 files)").arg(searchTerm).arg(totalHitCount).arg(currentFileCount));

    if (currentFile)
        currentFile->setText(0, QStringLiteral("%1 (%L2 hits)").arg(currentFilePath).arg(totalFileHitCount));
}

void SearchResultsDock::copySearchResultsToClipboard()
{
    QStringList results;
    QTreeWidgetItemIterator it(ui->treeWidget);

    while (*it) {
        const QTreeWidgetItem *item = *it;
        results.append(QStringLiteral("%1 %2").arg(item->text(0)).arg(item->text(1)));
        ++it;
    }

    QGuiApplication::clipboard()->setText(results.join('\n'));
}
