%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockSplitter : QSplitter
{
    %TypeHeaderCode
    #include <DockSplitter.h>
    %End

public:
	CDockSplitter(QWidget *parent /TransferThis/ = 0);
	CDockSplitter(Qt::Orientation orientation, QWidget *parent /TransferThis/ = 0);
	virtual ~CDockSplitter();
	bool hasVisibleContent() const;
	QWidget* firstWidget() const;
	QWidget* lastWidget() const;
    bool isResizingWithContainer() const;

};

};

%End
