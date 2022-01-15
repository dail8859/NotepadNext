/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
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


#include "DockedEditor.h"
#include "DockAreaWidget.h"
#include "DockWidgetTab.h"
#include "DockComponentsFactory.h"
#include "DockedEditorTitleBar.h"

#include "ScintillaNext.h"

#include <QUuid>


class DockedEditorComponentsFactory : public ads::CDockComponentsFactory
{
public:
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const {
        DockedEditorTitleBar *titleBar = new DockedEditorTitleBar(DockArea);

        // Disable the built in context menu for the title bar since it has options we don't want
        titleBar->setContextMenuPolicy(Qt::NoContextMenu);

        return titleBar;
    }
};


DockedEditor::DockedEditor(QWidget *parent) : QObject(parent)
{
    ads::CDockComponentsFactory::setFactory(new DockedEditorComponentsFactory());

    ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewIsDynamic, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::MiddleMouseButtonClosesTab, true);

    m_DockManager = new ads::CDockManager(parent);
    m_DockManager->setStyleSheet("");

    connect(m_DockManager, &ads::CDockManager::focusedDockWidgetChanged, this, [=](ads::CDockWidget* old, ads::CDockWidget* now) {
        Q_UNUSED(old)

        ScintillaNext *editor = qobject_cast<ScintillaNext *>(now->widget());

        currentEditor = editor;
        editor->grabFocus();
        emit editorActivated(editor);
    });

    connect(m_DockManager, &ads::CDockManager::dockAreaCreated, this, [=](ads::CDockAreaWidget* DockArea) {
        DockedEditorTitleBar *titleBar = qobject_cast<DockedEditorTitleBar *>(DockArea->titleBar());
        connect(titleBar, &DockedEditorTitleBar::doubleClicked, this, &DockedEditor::titleBarDoubleClicked);
    });
}


ScintillaNext *DockedEditor::getCurrentEditor() const
{
    return currentEditor;
}

int DockedEditor::count() const
{
    int total = 0;

    for (int i = 0; i < m_DockManager->dockAreaCount(); ++i)
        total += m_DockManager->dockArea(i)->dockWidgetsCount();

    return total;
}

QVector<ScintillaNext *> DockedEditor::editors() const
{
    QVector<ScintillaNext *> editors;

    for (const ads::CDockWidget* dockWidget : m_DockManager->dockWidgetsMap()) {
        editors.append(qobject_cast<ScintillaNext *>(dockWidget->widget()));
    }

    return editors;
}

void DockedEditor::switchToEditor(const ScintillaNext *editor)
{
    for (ads::CDockWidget* dockWidget : m_DockManager->dockWidgetsMap()) {
        ScintillaNext *dockedEditor = qobject_cast<ScintillaNext *>(dockWidget->widget());

        if (editor == dockedEditor) {
            dockWidget->raise();
            return;
        }
    }
}

void DockedEditor::dockWidgetCloseRequested()
{
    ads::CDockWidget *dockWidget = qobject_cast<ads::CDockWidget *>(sender());
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(dockWidget->widget());

    emit editorCloseRequested(editor);
}

ads::CDockAreaWidget *DockedEditor::currentDockArea() const
{
    QMap<QString, ads::CDockWidget*> dockwidgets = m_DockManager->dockWidgetsMap();

    if (dockwidgets.size() == 0) {
        return Q_NULLPTR;
    }
    else if (dockwidgets.size() == 1) {
        // If no dockwidget has had the focus set yet, just return the only one
        return dockwidgets.first()->dockAreaWidget();
    }

    // Search the list for the one that has had the focus set
    for (ads::CDockWidget* dockWidget : dockwidgets) {
        if (dockWidget->property("focused").toBool()) {
            return dockWidget->dockAreaWidget();
        }
    }

    // There was no area that had the focus
    return Q_NULLPTR;
}

void DockedEditor::addEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(editor != Q_NULLPTR);

    if (currentEditor == Q_NULLPTR) {
        currentEditor = editor;
    }

    emit editorAdded(editor);

    // Create the dock widget for the editor
    ads::CDockWidget *dw = new ads::CDockWidget(editor->getName());

    // We need a unique object name. Can't use the name or file path so use a uuid
    dw->setObjectName(QUuid::createUuid().toString());

    dw->setWidget(editor);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    dw->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetFloatable, false);

    dw->tabWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(dw->tabWidget(), &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        Q_UNUSED(pos)

        emit contextMenuRequestedForEditor(editor);
    });

    // Set the tooltip based on the buffer
    if (editor->isFile()) {
        dw->tabWidget()->setToolTip(editor->getFilePath());
    }
    else {
        dw->tabWidget()->setToolTip(editor->getName());
    }

    // Set the icon
    dw->tabWidget()->setIcon(QIcon(":/icons/saved.png"));
    connect(editor, &ScintillaNext::savePointChanged, dw, [=](bool dirty) {
        const QString iconPath = dirty ? ":/icons/unsaved.png" : ":/icons/saved.png";
        dw->tabWidget()->setIcon(QIcon(iconPath));
    });

    connect(editor, &ScintillaNext::closed, dw, &ads::CDockWidget::closeDockWidget);
    connect(editor, &ScintillaNext::renamed, this, [=]() { editorRenamed(editor); });

    connect(dw, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    m_DockManager->addDockWidget(ads::CenterDockWidgetArea, dw, currentDockArea());
}

void DockedEditor::editorRenamed(ScintillaNext *editor)
{
    Q_ASSERT(editor != Q_NULLPTR);

    ads::CDockWidget *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    const QString newName = editor->getName();

    dockWidget->setWindowTitle(newName);

    if (editor->isFile()) {
        dockWidget->tabWidget()->setToolTip(editor->getFilePath());
    }
    else {
        dockWidget->tabWidget()->setToolTip(editor->getName());
    }
}
