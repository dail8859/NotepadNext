#ifndef StatusDialogH
#define StatusDialogH
//============================================================================
/// \file   StatusDialog.h
/// \author Uwe Kindler
/// \date   13.04.2020
/// \brief  Declaration of CStatusDialog class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QDialog>

namespace ads {class CDockManager;}
struct StatusDialogPrivate;

/**
 * Displays status info about dock widgets
 */
class CStatusDialog : public QDialog
{
	Q_OBJECT
private:
	StatusDialogPrivate* d; ///< private data (pimpl)
    friend struct StatusDialogPrivate;

private slots:
	void on_dockWidgetsComboBox_currentIndexChanged(int index);

protected:
public:
	using Super = QDialog;
	/**
	 * Default Constructor
	 */
	CStatusDialog(ads::CDockManager* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CStatusDialog();
}; // class StatusDialog

 // namespace namespace_name
//-----------------------------------------------------------------------------
#endif // StatusDialogH
