%If (Qt_5_0_0 -)

namespace ads
{

class CDockingStateReader : QXmlStreamReader
{

    %TypeHeaderCode
    #include <DockingStateReader.h>
    %End


public:
	void setFileVersion(int FileVersion);
	int fileVersion() const;
};

};

%End