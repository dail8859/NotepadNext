#ifndef FloatingDragPreviewH
#define FloatingDragPreviewH
//============================================================================
/// \file   FloatingDragPreview.h
/// \author Uwe Kindler
/// \date   26.11.2019
/// \brief  Declaration of CFloatingDragPreview
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>
#include "FloatingDockContainer.h"

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
struct FloatingDragPreviewPrivate;

/**
 * A floating overlay is a temporary floating widget that is just used to
 * indicate the floating widget movement.
 * This widget is used as a placeholder for drag operations for non-opaque
 * docking
 */
class CFloatingDragPreview : public QWidget, public IFloatingWidget
{
	Q_OBJECT
private:
	FloatingDragPreviewPrivate* d;
	friend struct FloatingDragPreviewPrivate;

private slots:
	/**
	 * Cancel non opaque undocking if application becomes inactive
	 */
	void onApplicationStateChanged(Qt::ApplicationState state);

protected:
	/**
	 * Cares about painting the
	 */
	virtual void paintEvent(QPaintEvent *e) override;

	/**
	 * The content is a DockArea or a DockWidget
	 */
	CFloatingDragPreview(QWidget* Content, QWidget* parent);

public:
	using Super = QWidget;

	/**
	 * Creates an instance for undocking the DockWidget in Content parameter
	 */
	CFloatingDragPreview(CDockWidget* Content);

	/**
	 * Creates an instance for undocking the DockArea given in Content
	 * parameters
	 */
	CFloatingDragPreview(CDockAreaWidget* Content);

	/**
	 * Delete private data
	 */
	~CFloatingDragPreview();

    /**
     * We filter the events of the assigned content widget to receive
     * escape key presses for canceling the drag operation
     */
    virtual bool eventFilter(QObject *watched, QEvent *event) override;


public: // implements IFloatingWidget -----------------------------------------
	virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        eDragState DragState, QWidget* MouseEventHandler) override;

	/**
	 * Moves the widget to a new position relative to the position given when
	 * startFloating() was called
	 */
	virtual void moveFloating() override;

	/**
	 * Finishes dragging.
	 * Hides the dock overlays and executes the real undocking and docking
	 * of the assigned Content widget
	 */
	virtual void finishDragging() override;

signals:
	/**
	 * This signal is emitted, if dragging has been canceled by escape key
	 * or by active application switching via task manager
	 */
	void draggingCanceled();
};


} // namespace ads

//---------------------------------------------------------------------------
#endif // FloatingDragPreviewH

