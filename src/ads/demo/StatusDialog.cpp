//============================================================================
/// \file   StatusDialog.cpp
/// \author Uwe Kindler
/// \date   13.04.2020
/// \brief  Implementation of CStatusDialog class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "StatusDialog.h"

#include <iostream>

#include "DockManager.h"
#include "DockWidget.h"
#include "ui_StatusDialog.h"

/**
 * Private data class of CStatusDialog class (pimpl)
 */
struct StatusDialogPrivate
{
	CStatusDialog *_this;
	Ui::CStatusDialogClass ui;
	ads::CDockManager* DockManager;
	QMap<QString, ads::CDockWidget*> DockWidgets;

	/**
	 * Private data constructor
	 */
	StatusDialogPrivate(CStatusDialog *_public);
};
// struct StatusDialogPrivate

//============================================================================
StatusDialogPrivate::StatusDialogPrivate(CStatusDialog *_public) :
	_this(_public)
{

}

//============================================================================
CStatusDialog::CStatusDialog(ads::CDockManager* DockManager) :
	QDialog(DockManager),
	d(new StatusDialogPrivate(this))
{
	d->ui.setupUi(this);
	d->DockManager = DockManager;
	d->DockWidgets = DockManager->dockWidgetsMap();

	for (auto it = d->DockWidgets.begin(); it != d->DockWidgets.end(); ++it)
	{
		QVariant vDockWidget = QVariant::fromValue(it.value());
		d->ui.dockWidgetsComboBox->addItem(it.key(), vDockWidget);
	}
}

//============================================================================
CStatusDialog::~CStatusDialog()
{
	delete d;
}


//============================================================================
void CStatusDialog::on_dockWidgetsComboBox_currentIndexChanged(int index)
{
	if (index < 0)
	{
		return;
	}

	auto vDockWidget = d->ui.dockWidgetsComboBox->currentData();
	auto DockWidget = vDockWidget.value<ads::CDockWidget*>();
	d->ui.isClosedCheckBox->setChecked(DockWidget->isClosed());
	d->ui.isFloatingCheckBox->setChecked(DockWidget->isFloating());
	d->ui.tabbedCheckBox->setChecked(DockWidget->isTabbed());
	d->ui.isCurrentTabCheckBox->setChecked(DockWidget->isCurrentTab());
	d->ui.closableCheckBox->setChecked(DockWidget->features().testFlag(ads::CDockWidget::DockWidgetClosable));
	d->ui.movableCheckBox->setChecked(DockWidget->features().testFlag(ads::CDockWidget::DockWidgetMovable));
	d->ui.floatableCheckBox->setChecked(DockWidget->features().testFlag(ads::CDockWidget::DockWidgetFloatable));
	d->ui.deleteOnCloseCheckBox->setChecked(DockWidget->features().testFlag(ads::CDockWidget::DockWidgetDeleteOnClose));
	d->ui.customCloseHandlingCheckBox->setChecked(DockWidget->features().testFlag(ads::CDockWidget::CustomCloseHandling));
}

//---------------------------------------------------------------------------
// EOF StatusDialog.cpp
