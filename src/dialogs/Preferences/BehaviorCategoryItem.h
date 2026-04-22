#ifndef BEHAVIORCATEGORYITEM_H
#define BEHAVIORCATEGORYITEM_H

#include "PreferencesCategoryItem.h"

class BehaviorCategoryItem : public PreferencesCategoryItem
{
public:
    BehaviorCategoryItem() = default;
    virtual ~BehaviorCategoryItem() = default;

    virtual QString title() const override { return QObject::tr("Behavior"); }
    virtual QString icon() const override { return "://icons/audio-waveform.svg"; }
    virtual QWidget *view() const override;
};

#endif // BEHAVIORCATEGORYITEM_H
