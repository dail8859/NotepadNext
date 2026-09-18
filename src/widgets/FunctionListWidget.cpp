/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * Function list panel implementation.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#include "FunctionListWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPainter>
#include <QShortcut>
#include <QHash>

namespace {
QIcon makeKindIcon(const QColor &color, const QString &letter)
{
    QPixmap pm(16, 16);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawEllipse(2, 2, 12, 12);
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setBold(true);
    f.setPixelSize(10);
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, letter);
    return QIcon(pm);
}
} // namespace

FunctionListWidget::FunctionListWidget(QWidget *parent)
    : QWidget(parent)
{
    titleLabel = new QLabel(this);
    titleLabel->setObjectName(QStringLiteral("functionListTitle"));
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText(tr("Symbol Name (Alt+L)"));
    filterEdit->setClearButtonEnabled(true);

    // Hint shown while a parse runs, or when a file simply has no symbols.
    statusLabel = new QLabel(this);
    statusLabel->setObjectName(QStringLiteral("functionListStatus"));
    statusLabel->setWordWrap(true);
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    statusLabel->setVisible(false);
    {
        QFont statusFont = statusLabel->font();
        statusFont.setItalic(true);
        statusLabel->setFont(statusFont);
    }

    tree = new QTreeWidget(this);
    tree->setHeaderHidden(true);
    tree->setRootIsDecorated(false);
    tree->setUniformRowHeights(true);
    tree->setColumnCount(1);
    tree->setFrameShape(QFrame::NoFrame);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);
    layout->addWidget(titleLabel);
    layout->addWidget(filterEdit);
    layout->addWidget(statusLabel);
    layout->addWidget(tree, 1);

    connect(filterEdit, &QLineEdit::textChanged, this, &FunctionListWidget::onFilterChanged);
    connect(tree, &QTreeWidget::itemClicked, this, &FunctionListWidget::onItemClicked);

    // Alt+L focuses the filter box (matches the placeholder hint)
    QShortcut *focusFilter = new QShortcut(QKeySequence(QStringLiteral("Alt+L")), this);
    focusFilter->setContext(Qt::WidgetWithChildrenShortcut);
    connect(focusFilter, &QShortcut::activated, filterEdit, qOverload<>(&QLineEdit::setFocus));
}

void FunctionListWidget::setFileLabel(const QString &fileName)
{
    titleLabel->setText(fileName);
    titleLabel->setToolTip(fileName);
}

void FunctionListWidget::setSymbols(const QVector<FunctionSymbol> &symbols)
{
    QMutexLocker locker(&symbolsMutex);
    this->symbols = symbols;
    rebuildTree();
}

void FunctionListWidget::clearSymbols()
{
    QMutexLocker locker(&symbolsMutex);
    symbols.clear();
    rebuildTree();
}

void FunctionListWidget::setBusy(bool busy)
{
    QMutexLocker locker(&symbolsMutex);
    if (this->busy == busy)
        return;
    this->busy = busy;
    rebuildTree();
}

int FunctionListWidget::symbolCount() const
{
    QMutexLocker locker(&symbolsMutex);
    return symbols.size();
}

QString FunctionListWidget::currentFilterText() const
{
    return filterEdit->text();
}

void FunctionListWidget::onFilterChanged(const QString &text)
{
    QMutexLocker locker(&symbolsMutex);
    currentFilter = text;
    rebuildTree();
}

void FunctionListWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (!item)
        return;
    const int line = item->data(0, Qt::UserRole).toInt();
    if (line > 0)
        emit jumpToLineRequested(line);
}

void FunctionListWidget::rebuildTree()
{
    // NOTE: must be called with symbolsMutex held
    tree->clear();

    const QString filter = currentFilter.trimmed();

    for (const FunctionSymbol &sym : symbols) {
        if (!filter.isEmpty() && !sym.name.contains(filter, Qt::CaseInsensitive))
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(tree);
        item->setText(0, sym.name);
        item->setIcon(0, iconForKind(sym.kind));
        item->setData(0, Qt::UserRole, sym.line);
        item->setToolTip(0, QStringLiteral("%1  [line %2]").arg(sym.kind, QString::number(sym.line)));
    }

    // Explain an empty list instead of leaving a blank box behind: the panel is
    // opened as soon as ctags claims the file, so "symbols are still being
    // collected" and "this file has none" are two very different states.
    if (tree->topLevelItemCount() > 0) {
        statusLabel->setVisible(false);
        return;
    }

    if (busy)
        statusLabel->setText(tr("Analyzing..."));
    else if (!filter.isEmpty())
        statusLabel->setText(tr("No symbol matches \"%1\"").arg(filter));
    else
        statusLabel->setText(tr("No symbols found"));

    statusLabel->setVisible(true);
}

QIcon FunctionListWidget::iconForKind(const QString &kind)
{
    if (!kindIconsReady) {
        buildIconCache();
        kindIconsReady = true;
    }

    const auto it = kindIcons.constFind(kind);
    if (it != kindIcons.constEnd())
        return it.value();

    return makeKindIcon(QColor(0x6b7280), kind.isEmpty() ? QStringLiteral("?") : kind.left(1));
}

// Simple color coding similar to common function list panels
void FunctionListWidget::buildIconCache()
{
    kindIcons.insert(QStringLiteral("f"), makeKindIcon(QColor(0x2e9e4f), QStringLiteral("f"))); // function
    kindIcons.insert(QStringLiteral("p"), makeKindIcon(QColor(0x88b04b), QStringLiteral("p"))); // prototype/port
    kindIcons.insert(QStringLiteral("m"), makeKindIcon(QColor(0x1d7f9f), QStringLiteral("m"))); // member/module
    kindIcons.insert(QStringLiteral("v"), makeKindIcon(QColor(0xd97706), QStringLiteral("v"))); // variable
    kindIcons.insert(QStringLiteral("g"), makeKindIcon(QColor(0xd97706), QStringLiteral("g"))); // global var
    kindIcons.insert(QStringLiteral("e"), makeKindIcon(QColor(0xd97706), QStringLiteral("e"))); // enumerator
    kindIcons.insert(QStringLiteral("d"), makeKindIcon(QColor(0xc2419a), QStringLiteral("d"))); // macro
    kindIcons.insert(QStringLiteral("c"), makeKindIcon(QColor(0x2456c7), QStringLiteral("c"))); // class
    kindIcons.insert(QStringLiteral("s"), makeKindIcon(QColor(0x2456c7), QStringLiteral("s"))); // struct
    kindIcons.insert(QStringLiteral("u"), makeKindIcon(QColor(0x2456c7), QStringLiteral("u"))); // union
    kindIcons.insert(QStringLiteral("t"), makeKindIcon(QColor(0x7c3aed), QStringLiteral("t"))); // typedef
    kindIcons.insert(QStringLiteral("n"), makeKindIcon(QColor(0x0f766e), QStringLiteral("n"))); // namespace
    kindIcons.insert(QStringLiteral("M"), makeKindIcon(QColor(0x0f766e), QStringLiteral("M"))); // lua module
}
