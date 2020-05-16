%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{
    %TypeHeaderCode
    #include <linux/FloatingWidgetTitleBar.h>
    %End
    
class CFloatingWidgetTitleBar : QWidget
{

protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	virtual void mouseMoveEvent(QMouseEvent *ev);

public:
	explicit CFloatingWidgetTitleBar(CFloatingDockContainer *parent /TransferThis/ = 0);
	virtual ~CFloatingWidgetTitleBar();
	void enableCloseButton(bool Enable);
	void setTitle(const QString &Text);

signals:
	void closeRequested();
};

};

%End
