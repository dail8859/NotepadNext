%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CElidingLabel : QLabel
{
    %TypeHeaderCode
    #include <ElidingLabel.h>
    %End
    
protected:
	virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent( QResizeEvent *event );
    virtual void mouseDoubleClickEvent( QMouseEvent *ev );
    
public:
	CElidingLabel(QWidget* parent /TransferThis/ = 0, Qt::WindowFlags f = 0);
	CElidingLabel(const QString& text, QWidget* parent /TransferThis/ = 0, Qt::WindowFlags f = 0);
	virtual ~CElidingLabel();
	Qt::TextElideMode elideMode() const;
	void setElideMode(Qt::TextElideMode mode);
	bool isElided() const;

public:
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;
	void setText(const QString &text);
	QString text() const;

signals:
	void clicked();
	void doubleClicked();
	void elidedChanged(bool elided);
};

};

%End
