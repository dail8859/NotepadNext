#include <QDialogButtonBox>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QCheckBox>
#include <QListView>
#include <QSplitter>
#include <QLabel>

#include "Preferences/PreferencesCategoryListModel.h"
#include "Preferences/AppearanceCategoryItem.h"
#include "Preferences/BehaviorCategoryItem.h"
#include "Preferences/PreferencesViewUtils.h"
#include "PreferencesDialog.h"

using namespace Preferences;

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
        const auto tempSettings = createTemporarySettings(nullptr);
        if (tempSettings)
        {
            settings.actual->fillFrom(tempSettings);
            delete tempSettings;
        }
    }

    inline ApplicationSettings *createTemporarySettings(QObject *parent) const
    {
        const auto tempFile = new QTemporaryFile;

        if (!tempFile->open())
        {
            qWarning() << "Unable to create temporary file:"
                       << tempFile->errorString();
            delete tempFile;
            return nullptr;
        }

        return new ApplicationSettings(tempFile, parent);
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
        QWidget     *widget = nullptr;
        QVBoxLayout *layout = nullptr;

        QLabel      *title = nullptr;
        QScrollArea *viewport = nullptr;
    } content;

    QDialogButtonBox *controlsBox = nullptr;
};

PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent)
    : QDialog(parent, Qt::Tool),
      p(new PreferencesDialogPrivate)
{
    setWindowTitle(tr("Preferences"));

    p->settings.actual = settings;
    p->settings.backup = p->createTemporarySettings(this);

    // Category
    p->category.listView = new QListView;
    p->category.listView->setFixedWidth(180);
    p->category.listView->setIconSize({ 20, 20 });

    p->category.model = new ListModel(p->category.listView);
    p->category.model->addCategory(new BehaviorCategoryItem);
    p->category.model->addCategory(new AppearanceCategoryItem);

    p->category.listView->setModel(p->category.model);

    // Content
    p->content.title = new QLabel(tr("Default title"));
    p->content.title->setFont(ContentTitleFont());

    p->content.viewport = new QScrollArea;

    p->content.widget = new QWidget;
    p->content.widget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    p->content.layout = new QVBoxLayout(p->content.widget);
    p->content.layout->setContentsMargins(0, 0, 0, 0);
    p->content.layout->addWidget(p->content.title);
    p->content.layout->addWidget(p->content.viewport);

    // Controls
    p->controlsBox = new QDialogButtonBox(Qt::Horizontal);
    p->controlsBox->setStandardButtons(
        QDialogButtonBox::RestoreDefaults
        | QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel
    );

    // Main assembly
    p->mainLayout = new QGridLayout(this);
    p->mainLayout->addWidget(p->category.listView, 0, 0, 2, 1);
    p->mainLayout->addWidget(p->content.widget, 0, 1);
    p->mainLayout->addWidget(p->controlsBox, 1, 1);

    // SS-stuf
    connect(p->category.listView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this,                                   &PreferencesDialog::onCategoryChanged);

    connect(p->controlsBox, &QDialogButtonBox::clicked,
            this,           [this](QAbstractButton *button) {
        switch (p->controlsBox->buttonRole(button))
        {
        case QDialogButtonBox::ResetRole:  onResetClicked();  break;
        case QDialogButtonBox::AcceptRole: onOkClicked();     break;
        case QDialogButtonBox::RejectRole: onCancelClicked(); break;
        default: break;
        }
    });

    // Force switch to first category
    QMetaObject::invokeMethod(this, [this](){
        p->category.listView->setCurrentIndex(p->category.model->index(0));
    }, Qt::QueuedConnection);
}

PreferencesDialog::~PreferencesDialog()
{
    delete p;
}

void PreferencesDialog::showEvent(QShowEvent *event)
{
    p->makeSettingsBackup();
    QDialog::showEvent(event);
}

void PreferencesDialog::onCategoryChanged(const QModelIndex &index)
{
    if (!index.isValid()) return;

    const auto item = p->category.model->category(index.row());
    if (!item) return;

    p->content.title->setText(item->title());
    p->content.viewport->setWidget(item->contentView(p->settings.actual));
    p->content.viewport->widget()->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding
    );
    p->content.viewport->setWidgetResizable(true);
}

void PreferencesDialog::onOkClicked()
{
    accept();
}

void PreferencesDialog::onCancelClicked()
{
    if (p->hasUnsavedChanges()) {
        const auto reply = QMessageBox::question(
            this,
            tr("Unsaved Changes"),
            tr("You have unsaved changes.\n"
               "Do you want to save them before closing?"),
            QMessageBox::Save
            | QMessageBox::Discard
            | QMessageBox::Cancel
        );

        switch (reply)
        {
            case QMessageBox::Cancel:
                return;
            case QMessageBox::Discard:
                p->makeSettingsRestore();
                [[fallthrough]];
            default:
                break;
        }
    }

    accept();
}

void PreferencesDialog::onResetClicked()
{
    p->makeSettingsReset();
}
