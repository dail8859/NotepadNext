#include <QDialogButtonBox>
#include <QTemporaryFile>
#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QScrollArea>
#include <QCloseEvent>
#include <QTabWidget>
#include <QCheckBox>
#include <QListView>
#include <QSplitter>
#include <QLabel>

#include "Preferences/PreferencesCategoryListModel.h"
#include "Preferences/AppearanceCategoryItem.h"
#include "Preferences/BehaviorCategoryItem.h"
#include "PreferencesDialog.h"

#include "ApplicationSettings.h"

namespace
{
    using ListModel = PreferencesCategoryListModel;

    inline QFont ContentTitleFont() noexcept
    {
        auto font = QApplication::font("QLabel");
        font.setPointSize(16);
        font.setBold(true);
        return font;
    }
}

struct PreferencesDialog::PreferencesDialogPrivate
{
public:
    inline bool hasUnsavedChanges() const {
        return (settings.backup) ? !settings.actual->isEquals(settings.backup) : false;
    }
    inline void makeSettingsBackup() {
        if (settings.backup) settings.backup->fillFrom(settings.actual);
    }
    inline void makeSettingsRestore() {
        if (settings.backup) settings.actual->fillFrom(settings.backup);
    }
    inline void makeSettingsReset() const
    {
        std::unique_ptr<ApplicationSettings> tempSettings(createTemporarySettings(nullptr));
        if (tempSettings) settings.actual->fillFrom(tempSettings.get());
    }

    inline ApplicationSettings *createTemporarySettings(QObject *parent) const
    {
        auto tempFile = std::make_unique<QTemporaryFile>();

        if (!tempFile->open())
        {
            qWarning() << "Unable to create temporary file:"
                       << tempFile->errorString();
            return nullptr;
        }

        return new ApplicationSettings(tempFile.release(), parent);
    }

public: /* Logic */
    struct {
        /// @brief Actual settings populated in-app.
        ApplicationSettings *actual = nullptr;
        /// @brief Settings backup on latest show event.
        ApplicationSettings *backup = nullptr;
    } settings;

public: /* View */
    QGridLayout *mainLayout = nullptr;

    struct {
        QListView *listView = nullptr;
        ListModel *model = nullptr;
    } category;

    struct {
        QWidget *widget = nullptr;
        QVBoxLayout *layout = nullptr;

        QLabel *title = nullptr;
        QScrollArea *viewport = nullptr;
    } content;

    QDialogButtonBox *controlsBox = nullptr;
};

PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent)
    : QDialog(parent, Qt::Tool),
      d(new PreferencesDialogPrivate)
{
    setWindowTitle(tr("Preferences"));

    d->settings.actual = settings;
    d->settings.backup = d->createTemporarySettings(this);

    // Category
    d->category.listView = new QListView;
    d->category.listView->setFixedWidth(180);

    d->category.model = new ListModel(d->category.listView);
    d->category.model->addCategory(new BehaviorCategoryItem);
    d->category.model->addCategory(new AppearanceCategoryItem);

    d->category.listView->setModel(d->category.model);

    // Content
    d->content.title = new QLabel;
    d->content.title->setFont(ContentTitleFont());

    d->content.viewport = new QScrollArea;

    d->content.widget = new QWidget;
    d->content.widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->content.layout = new QVBoxLayout(d->content.widget);
    d->content.layout->setContentsMargins(0, 0, 0, 0);
    d->content.layout->addWidget(d->content.title);
    d->content.layout->addWidget(d->content.viewport);

    // Controls
    d->controlsBox = new QDialogButtonBox(Qt::Horizontal);
    d->controlsBox->setStandardButtons(
        QDialogButtonBox::RestoreDefaults
        | QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel
    );

    // Main assembly
    d->mainLayout = new QGridLayout(this);
    d->mainLayout->addWidget(d->category.listView, 0, 0, 2, 1);
    d->mainLayout->addWidget(d->content.widget, 0, 1);
    d->mainLayout->addWidget(d->controlsBox, 1, 1);

    connect(d->category.listView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &PreferencesDialog::onCategoryChanged);

    connect(d->controlsBox, &QDialogButtonBox::clicked, this, [this](QAbstractButton *button) {
        switch (d->controlsBox->buttonRole(button))
        {
            case QDialogButtonBox::ResetRole:  onResetClicked();  break;
            case QDialogButtonBox::AcceptRole: onOkClicked();     break;
            case QDialogButtonBox::RejectRole: onCancelClicked(); break;
            default: break;
        }
    });

    // Force switch to first category
    QMetaObject::invokeMethod(this, [this](){
        d->category.listView->setCurrentIndex(d->category.model->index(0));
    }, Qt::QueuedConnection);
}

PreferencesDialog::~PreferencesDialog()
{
    delete d;
}

void PreferencesDialog::showEvent(QShowEvent *event)
{
    d->makeSettingsBackup();
    QDialog::showEvent(event);
}

void PreferencesDialog::closeEvent(QCloseEvent *event)
{
    if (isVisible() && d->hasUnsavedChanges())
    {
        const auto reply = QMessageBox::question(
            this,
            tr("Unsaved сhanges"),
            tr("You have unsaved changes.\n"
               "Do you want to save them before closing?"),
            QMessageBox::Save
            | QMessageBox::Discard
            | QMessageBox::Cancel
        );

        switch (reply)
        {
            case QMessageBox::Cancel:
                event->ignore();
                return;
            case QMessageBox::Discard:
                d->makeSettingsRestore();
                [[fallthrough]];
            case QMessageBox::Save:
                d->settings.actual->sync();
                break;
            default:
                break;
        }
    }

    QDialog::closeEvent(event);
}

void PreferencesDialog::onCategoryChanged(const QModelIndex &index)
{
    if (!index.isValid()) return;

    const auto item = d->category.model->category(index.row());
    if (!item) return;

    d->content.title->setText(item->title());
    d->content.viewport->setWidget(item->contentView(d->settings.actual));
    d->content.viewport->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->content.viewport->setWidgetResizable(true);
}

void PreferencesDialog::onOkClicked()
{
    d->settings.actual->sync();
    accept();
}

void PreferencesDialog::onCancelClicked()
{
    close();
}

void PreferencesDialog::onResetClicked()
{
    d->makeSettingsReset();
}
