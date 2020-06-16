%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CTitleBarButton : QToolButton
{
    %TypeHeaderCode
    #include <DockAreaTitleBar_p.h>
    %End

protected:
    bool event(QEvent *ev);

public:
    CTitleBarButton(bool visible = true, QWidget* parent /TransferThis/ = Q_NULLPTR );
    virtual void setVisible(bool visible);
};


class CSpacerWidget : QWidget
{
    %TypeHeaderCode
    #include <DockAreaTitleBar_p.h>
    %End

public:
	CSpacerWidget(QWidget* Parent /TransferThis/ = 0 );
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
};

};

%End
