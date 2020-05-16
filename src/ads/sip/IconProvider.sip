%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CIconProvider
{

    %TypeHeaderCode
    #include <IconProvider.h>
    %End


public:
    CIconProvider();

  	virtual ~CIconProvider();

    QIcon customIcon(eIcon IconId);

    void registerCustomIcon(eIcon IconId, const QIcon& icon /TransferThis/ );
};

};

%End