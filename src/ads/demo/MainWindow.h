#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   MainWindow.h
/// \author Uwe Kindler
/// \date   13.02.2018
/// \brief  Declaration of CMainWindow class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QMainWindow>



struct MainWindowPrivate;


/**
 * Simple main window for demo
 */
class CMainWindow : public QMainWindow
{
	Q_OBJECT
private:
	MainWindowPrivate* d;///< private data - pimpl
	friend struct MainWindowPrivate;

protected:
	virtual void closeEvent(QCloseEvent* event) override;

public:
	explicit CMainWindow(QWidget *parent = 0);
	virtual ~CMainWindow();

private slots:
	void on_actionSaveState_triggered(bool);
	void on_actionRestoreState_triggered(bool);
	void savePerspective();
	void onViewToggled(bool Open);
	void onViewVisibilityChanged(bool Visible);
	void createEditor();
	void createTable();
	void onEditorCloseRequested();
	void showStatusDialog();
	void toggleDockWidgetWindowTitle();
};

#endif // MAINWINDOW_H
