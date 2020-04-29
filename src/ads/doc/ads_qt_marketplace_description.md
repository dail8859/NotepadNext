# Advanced Docking System for Qt

Qt Advanced Docking System lets you create customizable layouts using a full
featured window docking system similar to what is found in many popular
integrated development environments (IDEs) such as Visual Studio.

[![Video Advanced Docking](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/advanced-docking_video.png)](https://www.youtube.com/watch?v=7pdNfafg3Qc)

Everything is implemented with standard Qt functionality without any
platform specific code. Basic usage of QWidgets and QLayouts and using basic 
styles as much as possible.

## Features

### Overview

- [Features](#features)
  - [Overview](#overview)
  - [Docking everywhere - no central widget](#docking-everywhere---no-central-widget)
  - [Docking inside floating windows](#docking-inside-floating-windows)
  - [Grouped dragging](#grouped-dragging)
  - [Perspectives for fast switching of the complete main window layout](#perspectives-for-fast-switching-of-the-complete-main-window-layout)
  - [Opaque and non-opaque splitter resizing](#opaque-and-non-opaque-splitter-resizing)
  - [Opaque and non-opaque undocking](#opaque-and-non-opaque-undocking)
  - [Tab-menu for easy handling of many tabbed dock widgets](#tab-menu-for-easy-handling-of-many-tabbed-dock-widgets)
  - [Many different ways to detach dock widgets](#many-different-ways-to-detach-dock-widgets)
  - [Supports deletion of dynamically created dock widgets](#supports-deletion-of-dynamically-created-dock-widgets)

### Docking everywhere - no central widget

There is no central widget like in the Qt docking system. You can dock on every
border of the main window or you can dock into each dock area - so you are
free to dock almost everywhere.

![Dropping widgets](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/preview-dragndrop.png)

![Dropping widgets](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/preview-dragndrop_dark.png)

### Docking inside floating windows

There is no difference between the main window and a floating window. Docking
into floating windows is supported.

![Docking inside floating windows](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/floating-widget-dragndrop.png)

![Docking inside floating windows](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/floating-widget-dragndrop_dark.png)

### Grouped dragging

When dragging the titlebar of a dock, all the tabs that are tabbed with it are 
going to be dragged. So you can move complete groups of tabbed widgets into
a floating widget or from one dock area to another one.

![Grouped dragging](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/grouped-dragging.gif)

![Grouped dragging](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/grouped-dragging_dark.png)

### Perspectives for fast switching of the complete main window layout

A perspective defines the set and layout of dock windows in the main
window. You can save the current layout of the dockmanager into a named
perspective to make your own custom perspective. Later you can simply
select a perspective from the perspective list to quickly switch the complete 
main window layout.

![Perspective](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/perspectives.gif)

![Perspective](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/perspectives_dark.png)

### Opaque and non-opaque splitter resizing

The advanced docking system uses standard QSplitters as resize separators and thus supports opaque and non-opaque resizing functionality of QSplitter. In some rare cases, for very complex widgets or on slow machines resizing via separator on the fly may cause flicking and glaring of rendered content inside a widget. The global dock manager flag `OpaqueSplitterResize` configures the resizing behaviour of the splitters. If this flag is set, then widgets are resized dynamically (opaquely) while interactively moving the splitters. 

![Opaque resizing](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/opaque_resizing.gif)

If this flag is cleared, the widget resizing is deferred until the mouse button is released - this is some kind of lazy resizing separator.

![Non-opaque resizing](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/non_opaque_resizing.gif)

### Opaque and non-opaque undocking

By default, opaque undocking is active. That means, as soon as you drag a dock widget or a dock area with a number of dock widgets it will be undocked and moved into a floating widget and then the floating widget will be dragged around. That means undocking will take place immediatelly. You can compare this with opaque splitter resizing. If the flag `OpaqueUndocking` is cleared, then non-opaque undocking is active. In this mode, undocking is more like a standard drag and drop operation. That means, the dragged dock widget or dock area is not undocked immediatelly. Instead, a drag preview widget is created and dragged around to indicate the future position of the dock widget or dock area. The actual dock operation is only executed when the mouse button is released. That makes it possible, to cancel an active drag operation with the escape key.

The drag preview widget can be configured by a number of global dock manager flags:

- `DragPreviewIsDynamic`: if this flag is enabled, the preview will be adjusted dynamically to the drop area
- `DragPreviewShowsContentPixmap`: the created drag preview window shows a static copy of the content of the dock widget / dock are that is dragged
- `DragPreviewHasWindowFrame`: this flag configures if the drag preview is frameless like a QRubberBand or looks like a real window

The best way to test non-opaque undocking is to set the standard flags: `CDockManager::setConfigFlags(CDockManager::DefaultNonOpaqueConfig)`.

### Tab-menu for easy handling of many tabbed dock widgets

Tabs are a good way to quickly switch between dockwidgets in a dockarea. However, if the number of dockwidgets in a dockarea is too large, this may affect the usability of the tab bar. To keep track in this situation, you can use the tab menu. The menu allows you to quickly select the dockwidget you want to activate from a drop down menu.

![Tab menu](https://raw.githubusercontent.com/githubuser0xFFFF/Qt-Advanced-Docking-System/master/doc/tab_menu.gif)

### Many different ways to detach dock widgets

You can detach dock widgets and also dock areas in the following ways:

- by dragging the dock widget tab or the dock area title bar
- by double clicking the tab or title bar
- by using the detach menu entry from the tab and title bar drop down menu

### Supports deletion of dynamically created dock widgets

Normally clicking the close button of a dock widget will just hide the widget and the user can show it again using the toggleView() action of the dock widget. This is meant for user interfaces with a static amount of widgets. But the advanced docking system also supports dynamic dock widgets that will get deleted on close. If you set the dock widget flag `DockWidgetDeleteOnClose` for a certain dock widget, then it will be deleted as soon as you close this dock widget. This enables the implementation of user interfaces with dynamically created editors, like in word processing applications or source code development tools.
