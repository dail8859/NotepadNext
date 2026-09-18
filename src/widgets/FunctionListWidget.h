/*
 * This file is part of Notepad Next.
 * Copyright 2026 Notepad Next contributors
 *
 * Function list panel: file name label + symbol filter box + symbol list.
 * Attached to the left side of the editor inside an EditorPane splitter.
 *
 * This file is part of a feature licensed under the GNU General Public
 * License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 */

#ifndef FUNCTIONLISTWIDGET_H
#define FUNCTIONLISTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QHash>
#include <QIcon>
#include <QMutex>

class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;

struct FunctionSymbol
{
    QString name;
    QString kind;
    int line = 0; // 1-based line number
};

Q_DECLARE_METATYPE(FunctionSymbol)

class FunctionListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionListWidget(QWidget *parent = nullptr);

    void setFileLabel(const QString &fileName);

    // Replace the whole symbol list. Safe against concurrent access:
    // the internal buffer is guarded by a mutex and re-applies the
    // current filter text after the rebuild.
    void setSymbols(const QVector<FunctionSymbol> &symbols);

    void clearSymbols();

    // Marks the panel as "a parse is on its way". The panel is opened as soon as
    // ctags claims the file, long before the symbols exist, so this is what turns
    // the otherwise empty list into a hint that something is happening.
    void setBusy(bool busy);

    // Number of symbols currently held (unfiltered)
    int symbolCount() const;

    QString currentFilterText() const;

signals:
    // 1-based line number of the clicked symbol
    void jumpToLineRequested(int lineNumber);

private slots:
    void onFilterChanged(const QString &text);
    void onItemClicked(QTreeWidgetItem *item, int column);

private:
    void rebuildTree();
    QIcon iconForKind(const QString &kind);
    void buildIconCache();

    QLabel *titleLabel = nullptr;
    QLineEdit *filterEdit = nullptr;
    QLabel *statusLabel = nullptr; // "analyzing" / "no symbols" hint under the filter box
    QTreeWidget *tree = nullptr;

    // Kind icons live in the widget instead of a function local static: static
    // QIcon/QPixmap objects are destroyed after QGuiApplication, which is a
    // known crash source on macOS and Linux, and the cache is also guaranteed
    // to be built on the GUI thread.
    QHash<QString, QIcon> kindIcons;
    bool kindIconsReady = false;

    mutable QMutex symbolsMutex; // guards symbols/currentFilter/busy (thread-safe refresh requirement)
    QVector<FunctionSymbol> symbols;
    QString currentFilter;
    bool busy = false;
};

#endif // FUNCTIONLISTWIDGET_H
