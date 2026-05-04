#ifndef PREFERENCESVIEWUTILS_H
#define PREFERENCESVIEWUTILS_H

#include <QFileSystemModel>
#include <QApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCompleter>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QEvent>

#include <ApplicationSettings.h>

#define PREFERENCES_BIND_PROPERTY(name, Name) \
    &ApplicationSettings::name, \
    &ApplicationSettings::set##Name, \
    &ApplicationSettings::name##Changed

namespace Preferences
{
    inline const QMargins MARGINS_0;
    inline const QMargins MARGINS_6(6, 6, 6, 6);

    inline const int SPACING_0(0);
    inline const int SPACING_3(3);
    inline const int SPACING_6(6);

    class WheelEventSuppressFilter : public QObject
    {
    public:
        explicit WheelEventSuppressFilter(QObject *parent = nullptr)
            : QObject(parent) { }

        inline void setupFor(QWidget *widget)
        {
            widget->installEventFilter(this);
            widget->setFocusPolicy(Qt::ClickFocus);
        }

        inline virtual bool eventFilter(QObject *obj, QEvent *event) override
        {
            if (event->type() == QEvent::Wheel)
            {
                const auto widget = qobject_cast<QWidget*>(obj);
                if (widget && !widget->hasFocus())
                {
                    event->ignore();
                    return true;
                }
            }

            return QObject::eventFilter(obj, event);
        }
    };

    class RestartRequiredLabel : public QWidget
    {
    public:
        RestartRequiredLabel(QWidget *parent = nullptr)
            : QWidget(parent)
        {
            const auto iconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);

            const auto icon = new QLabel;
            icon->setPixmap(
                qApp->style()->standardIcon(
                    QStyle::SP_MessageBoxInformation
                ).pixmap(iconSize, iconSize)
            );

            mLabel = new QLabel(QObject::tr("Application restart required to apply changes."));
            mLabel->setWordWrap(true);

            auto infoFont = mLabel->font();
            infoFont.setPointSize(infoFont.pointSize() - 1);
            infoFont.setItalic(true);
            mLabel->setFont(infoFont);

            const auto layout = new QHBoxLayout(this);
            layout->addWidget(icon, 0, Qt::AlignTop);
            layout->addWidget(mLabel, 1);
            layout->setContentsMargins(9, 0, 3, 6);
            layout->setSpacing(SPACING_3);
        }

        inline const QLabel *label() const {
            return mLabel;
        }

    private:
        QLabel *mLabel = nullptr;
    };

#if QT_CONFIG(completer)
    inline QCompleter *FilesystemCompliter(QObject *parent,
                                           Qt::CaseSensitivity caseSensivity = Qt::CaseInsensitive,
                                           QCompleter::CompletionMode completionMode = QCompleter::InlineCompletion,
                                           const QDir::Filters &filters = QDir::AllDirs | QDir::NoDotAndDotDot)
    {
        const auto fsModel = new QFileSystemModel(parent);
        fsModel->setRootPath("");
        fsModel->setFilter(filters);

        const auto completer = new QCompleter(fsModel, parent);
        completer->setCompletionMode(completionMode);
        completer->setCaseSensitivity(caseSensivity);

        return completer;
    }
#endif

    template<typename G, typename S, typename N>
    inline QCheckBox *CreateCheckBox(const QString &title,
                                     ApplicationSettings *settings,
                                     G getter, S setter, N notifier,
                                     const QString &toolTip = QString())
    {
        const auto checkBox = new QCheckBox(title);

        if (!toolTip.isEmpty())
            checkBox->setToolTip(toolTip);

        checkBox->setChecked(std::bind(getter, settings)());

        QObject::connect(settings, notifier, checkBox, &QCheckBox::setChecked);
        QObject::connect(checkBox, &QCheckBox::toggled, settings, setter);

        return checkBox;
    }
}

#endif // PREFERENCESVIEWUTILS_H
