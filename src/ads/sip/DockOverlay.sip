%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockOverlay : public QFrame
{
    %TypeHeaderCode
    #include <DockOverlay.h>
    %End

public:

	enum eMode
	{
		ModeDockAreaOverlay,
		ModeContainerOverlay
	};

	CDockOverlay(QWidget* parent /TransferThis/, eMode Mode = ads::CDockOverlay::ModeDockAreaOverlay);
	virtual ~CDockOverlay();
	void setAllowedAreas(ads::DockWidgetAreas areas);
	ads::DockWidgetAreas allowedAreas() const;
	ads::DockWidgetArea dropAreaUnderCursor() const;
    ads::DockWidgetArea visibleDropAreaUnderCursor() const;
	ads::DockWidgetArea showOverlay(QWidget* target);
	void hideOverlay();
	void enableDropPreview(bool Enable);
    bool dropPreviewEnabled() const;
	QRect dropOverlayRect() const;
	virtual bool event(QEvent *e);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void showEvent(QShowEvent* e);
	virtual void hideEvent(QHideEvent* e);
};


class CDockOverlayCross : public QWidget
{

public:
	enum eIconColor
	{
		FrameColor,
		WindowBackgroundColor,
		OverlayColor,
		ArrowColor,
		ShadowColor
	};
    
protected:
    QString iconColors() const;
    QColor iconColor() const;
    void setIconFrameColor(const QColor& Color);
    void setIconBackgroundColor(const QColor& Color);
    void setIconOverlayColor(const QColor& Color);
    void setIconArrowColor(const QColor& Color);
    void setIconShadowColor(const QColor& Color);
    virtual void showEvent(QShowEvent* e);
    void setAreaWidgets(const QHash<ads::DockWidgetArea, QWidget*>& widgets);

public:
	CDockOverlayCross(ads::CDockOverlay* overlay /TransferThis/);
	virtual ~CDockOverlayCross();
	void setIconColor(ads::CDockOverlayCross::eIconColor ColorIndex, const QColor& Color);
	QColor iconColor(ads::CDockOverlayCross::eIconColor ColorIndex) const;
	ads::DockWidgetArea cursorLocation() const;
	void setupOverlayCross(ads::CDockOverlay::eMode Mode);
	void updateOverlayIcons();
	void reset();
	void updatePosition();
    void setIconColors(const QString& Colors);
};

};

%End
