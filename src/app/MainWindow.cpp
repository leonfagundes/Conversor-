#include "MainWindow.h"

#include "core/ConversionPlan.h"
#include "core/EngineCommand.h"
#include "core/FormatRegistry.h"
#include "core/ThemePreference.h"

#include <QApplication>
#include <QComboBox>
#include <QAbstractItemView>
#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPalette>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStyleHints>
#include <QStandardPaths>
#include <QTableWidget>
#include <QTabWidget>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QWidget>

#include <array>
#include <stdexcept>

namespace {

std::filesystem::path toPath(const QString& value)
{
#ifdef _WIN32
    return std::filesystem::path(value.toStdWString());
#else
    return std::filesystem::path(value.toStdString());
#endif
}

QString fromPath(const std::filesystem::path& path)
{
#ifdef _WIN32
    return QString::fromStdWString(path.wstring());
#else
    return QString::fromStdString(path.string());
#endif
}

QString categoryLabel(conversor::FileCategory category)
{
    return QString::fromStdString(conversor::categoryName(category));
}

QString executableName(const QString& key)
{
#ifdef _WIN32
    return key + ".exe";
#else
    return key;
#endif
}

QStringList toQStringList(const std::vector<std::string>& values)
{
    QStringList result;
    for (const auto& value : values) {
        result.append(QString::fromStdString(value));
    }
    return result;
}

QString themeLabel(conversor::ThemePreference preference)
{
    switch (preference) {
    case conversor::ThemePreference::System:
        return "Sistema";
    case conversor::ThemePreference::Light:
        return "Claro";
    case conversor::ThemePreference::Dark:
        return "Escuro";
    }

    return "Sistema";
}

std::optional<conversor::EffectiveTheme> detectedSystemTheme()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto* hints = QGuiApplication::styleHints();
    if (hints == nullptr) {
        return std::nullopt;
    }

    switch (hints->colorScheme()) {
    case Qt::ColorScheme::Dark:
        return conversor::EffectiveTheme::Dark;
    case Qt::ColorScheme::Light:
        return conversor::EffectiveTheme::Light;
    case Qt::ColorScheme::Unknown:
        return std::nullopt;
    }
#endif

    return std::nullopt;
}

void setPaletteColors(QPalette& palette,
                      QPalette::ColorRole role,
                      const QColor& active,
                      const QColor& disabled)
{
    palette.setColor(QPalette::Active, role, active);
    palette.setColor(QPalette::Inactive, role, active);
    palette.setColor(QPalette::Disabled, role, disabled);
}

QPalette lightPalette()
{
    QPalette palette;
    const QColor window(240, 240, 240);
    const QColor panel(255, 255, 255);
    const QColor text(25, 25, 25);
    const QColor disabledText(128, 128, 128);
    const QColor button(240, 240, 240);
    const QColor highlight(0, 120, 215);

    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::Base, panel);
    palette.setColor(QPalette::AlternateBase, QColor(247, 247, 247));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 225));
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, highlight);
    palette.setColor(QPalette::Highlight, highlight);
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::PlaceholderText, QColor(96, 96, 96));

    setPaletteColors(palette, QPalette::WindowText, text, disabledText);
    setPaletteColors(palette, QPalette::Text, text, disabledText);
    setPaletteColors(palette, QPalette::ButtonText, text, disabledText);

    return palette;
}

QPalette darkPalette()
{
    QPalette palette;
    const QColor window(32, 32, 32);
    const QColor panel(25, 25, 25);
    const QColor text(235, 235, 235);
    const QColor disabledText(132, 132, 132);
    const QColor button(45, 45, 45);
    const QColor highlight(0, 120, 215);

    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::Base, panel);
    palette.setColor(QPalette::AlternateBase, QColor(38, 38, 38));
    palette.setColor(QPalette::ToolTipBase, QColor(45, 45, 45));
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::BrightText, QColor(255, 255, 255));
    palette.setColor(QPalette::Link, QColor(96, 170, 255));
    palette.setColor(QPalette::Highlight, highlight);
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    palette.setColor(QPalette::PlaceholderText, QColor(160, 160, 160));

    setPaletteColors(palette, QPalette::WindowText, text, disabledText);
    setPaletteColors(palette, QPalette::Text, text, disabledText);
    setPaletteColors(palette, QPalette::ButtonText, text, disabledText);

    return palette;
}

QString classicStyleSheet(conversor::EffectiveTheme theme)
{
    if (theme == conversor::EffectiveTheme::Dark) {
        return R"(
QToolTip {
    background-color: #2d2d2d;
    border: 1px solid #6b6b6b;
    color: #ebebeb;
}
QGroupBox {
    border: 1px solid #6b6b6b;
    margin-top: 10px;
    padding: 6px 8px 8px 8px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 8px;
    padding: 0 4px;
    font-weight: 600;
}
QLineEdit,
QComboBox,
QTableWidget,
QPlainTextEdit {
    border: 1px solid #6b6b6b;
    background-color: #191919;
    color: #ebebeb;
}
QTableWidget,
QPlainTextEdit {
    gridline-color: #565656;
}
QHeaderView::section {
    background-color: #2d2d2d;
    border: 0;
    border-right: 1px solid #6b6b6b;
    border-bottom: 1px solid #6b6b6b;
    color: #ebebeb;
    padding: 4px;
}
QTabWidget::pane {
    border: 1px solid #6b6b6b;
    top: -1px;
}
QTabBar::tab {
    background-color: #2d2d2d;
    border: 1px solid #6b6b6b;
    color: #ebebeb;
    margin-right: 2px;
    padding: 4px 18px;
}
QTabBar::tab:selected {
    background-color: #191919;
    border-bottom-color: #191919;
}
QProgressBar {
    border: 1px solid #6b6b6b;
    background-color: #191919;
    color: #ebebeb;
    min-height: 18px;
    text-align: center;
}
QProgressBar::chunk {
    background-color: #16833a;
}
QPlainTextEdit {
    font-family: Consolas, monospace;
}
QPlainTextEdit#logView {
    border: 1px solid #6b6b6b;
    background-color: #191919;
    color: #ebebeb;
    selection-background-color: #005a9e;
    selection-color: #ffffff;
}
)";
    }

    return R"(
QToolTip {
    background-color: #ffffe1;
    border: 1px solid #767676;
    color: #191919;
}
QGroupBox {
    border: 1px solid #b8b8b8;
    margin-top: 10px;
    padding: 6px 8px 8px 8px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 8px;
    padding: 0 4px;
    font-weight: 600;
}
QLineEdit,
QComboBox,
QTableWidget,
QPlainTextEdit {
    border: 1px solid #b8b8b8;
    background-color: #ffffff;
    color: #191919;
}
QTableWidget,
QPlainTextEdit {
    gridline-color: #d6d6d6;
}
QHeaderView::section {
    background-color: #f0f0f0;
    border: 0;
    border-right: 1px solid #b8b8b8;
    border-bottom: 1px solid #b8b8b8;
    color: #191919;
    padding: 4px;
}
QTabWidget::pane {
    border: 1px solid #b8b8b8;
    top: -1px;
}
QTabBar::tab {
    background-color: #f0f0f0;
    border: 1px solid #b8b8b8;
    color: #191919;
    margin-right: 2px;
    padding: 4px 18px;
}
QTabBar::tab:selected {
    background-color: #ffffff;
    border-bottom-color: #ffffff;
}
QProgressBar {
    border: 1px solid #b8b8b8;
    background-color: #ffffff;
    color: #191919;
    min-height: 18px;
    text-align: center;
}
QProgressBar::chunk {
    background-color: #16a34a;
}
QPlainTextEdit {
    font-family: Consolas, monospace;
}
QPlainTextEdit#logView {
    border: 1px solid #b8b8b8;
    background-color: #ffffff;
    color: #191919;
    selection-background-color: #0078d7;
    selection-color: #ffffff;
}
)";
}

const std::array<conversor::FileCategory, 4> visibleCategories = {
    conversor::FileCategory::Image,
    conversor::FileCategory::Audio,
    conversor::FileCategory::Video,
    conversor::FileCategory::Document,
};

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Conversor");
    resize(860, 620);

    const auto savedTheme = QSettings().value("appearance/theme", "system").toString().toStdString();
    themePreference_ = conversor::themePreferenceFromKey(savedTheme).value_or(conversor::ThemePreference::System);

    const auto documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    outputDirectory_ = toPath(documents.isEmpty() ? QDir::homePath() : documents) / "Conversor Output";

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    auto* filesGroup = new QGroupBox("Arquivos", central);
    auto* filesLayout = new QGridLayout(filesGroup);
    filesLayout->setContentsMargins(8, 8, 8, 8);
    filesLayout->setSpacing(6);

    auto* addFilesButton = new QPushButton("Adicionar arquivos...", filesGroup);
    auto* addFolderButton = new QPushButton("Adicionar pasta...", filesGroup);
    auto* clearButton = new QPushButton("Limpar lista", filesGroup);
    summaryLabel_ = new QLabel("Nenhum arquivo carregado", filesGroup);

    const std::array themePreferences = {
        conversor::ThemePreference::System,
        conversor::ThemePreference::Light,
        conversor::ThemePreference::Dark,
    };

    fileTable_ = new QTableWidget(0, 5, filesGroup);
    fileTable_->setHorizontalHeaderLabels({"Arquivo", "Categoria", "Formato", "Tamanho", "Status"});
    fileTable_->horizontalHeader()->setStretchLastSection(true);
    fileTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    fileTable_->verticalHeader()->setVisible(false);
    fileTable_->setAlternatingRowColors(true);
    fileTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    fileTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    filesLayout->addWidget(addFilesButton, 0, 0);
    filesLayout->addWidget(addFolderButton, 0, 1);
    filesLayout->addWidget(clearButton, 0, 2);
    filesLayout->addWidget(summaryLabel_, 1, 0, 1, 3);
    filesLayout->addWidget(fileTable_, 2, 0, 1, 3);
    filesLayout->setColumnStretch(2, 1);
    root->addWidget(filesGroup, 1);

    auto* destinationGroup = new QGroupBox("Destino", central);
    auto* destinationLayout = new QGridLayout(destinationGroup);
    destinationLayout->setContentsMargins(8, 8, 8, 8);
    destinationLayout->setSpacing(6);

    auto* outputTextLabel = new QLabel("Pasta de saida", destinationGroup);
    auto* outputButton = new QPushButton("Selecionar...", destinationGroup);
    auto* themeTextLabel = new QLabel("Tema", destinationGroup);
    outputPathEdit_ = new QLineEdit(fromPath(outputDirectory_), destinationGroup);
    outputPathEdit_->setReadOnly(true);
    outputPathEdit_->setMinimumWidth(320);
    themeCombo_ = new QComboBox(destinationGroup);
    themeCombo_->setMinimumWidth(140);

    for (const auto preference : themePreferences) {
        themeCombo_->addItem(
            themeLabel(preference),
            QString::fromStdString(conversor::themePreferenceKey(preference)));
        if (preference == themePreference_) {
            themeCombo_->setCurrentIndex(themeCombo_->count() - 1);
        }
    }

    destinationLayout->addWidget(outputTextLabel, 0, 0);
    destinationLayout->addWidget(outputPathEdit_, 0, 1);
    destinationLayout->addWidget(outputButton, 0, 2);
    destinationLayout->addWidget(themeTextLabel, 1, 0);
    destinationLayout->addWidget(themeCombo_, 1, 1);
    destinationLayout->setColumnStretch(1, 1);
    root->addWidget(destinationGroup);

    auto* formatGroup = new QGroupBox("Opcoes de conversao", central);
    auto* formatLayout = new QVBoxLayout(formatGroup);
    formatLayout->setContentsMargins(8, 8, 8, 8);
    formatLayout->setSpacing(6);

    categoryTabs_ = new QTabWidget(formatGroup);
    for (const auto category : visibleCategories) {
        auto* tab = new QWidget(categoryTabs_);
        configureCategoryTab(category, tab);
        categoryTabs_->addTab(tab, categoryLabel(category));
    }
    formatLayout->addWidget(categoryTabs_);
    root->addWidget(formatGroup);

    auto* statusGroup = new QGroupBox("Status", central);
    auto* statusLayout = new QGridLayout(statusGroup);
    statusLayout->setContentsMargins(8, 8, 8, 8);
    statusLayout->setSpacing(6);

    progressBar_ = new QProgressBar(statusGroup);
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    convertButton_ = new QPushButton("Converter", statusGroup);
    cancelButton_ = new QPushButton("Cancelar", statusGroup);
    convertButton_->setDefault(true);
    cancelButton_->setEnabled(false);

    logView_ = new QPlainTextEdit(statusGroup);
    logView_->setObjectName("logView");
    logView_->setReadOnly(true);
    logView_->setMaximumBlockCount(500);
    logView_->setPlaceholderText("Log de conversao");
    logView_->setMinimumHeight(92);

    statusLayout->addWidget(progressBar_, 0, 0);
    statusLayout->addWidget(convertButton_, 0, 1);
    statusLayout->addWidget(cancelButton_, 0, 2);
    statusLayout->addWidget(logView_, 1, 0, 1, 3);
    statusLayout->setColumnStretch(0, 1);
    root->addWidget(statusGroup);

    setCentralWidget(central);

    connect(addFilesButton, &QPushButton::clicked, this, [this] { addFiles(); });
    connect(addFolderButton, &QPushButton::clicked, this, [this] { addFolder(); });
    connect(outputButton, &QPushButton::clicked, this, [this] { chooseOutputDirectory(); });
    connect(clearButton, &QPushButton::clicked, this, [this] { clearFiles(); });
    connect(themeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        const auto preference = conversor::themePreferenceFromKey(
            themeCombo_->itemData(index).toString().toStdString());
        if (preference.has_value()) {
            updateThemePreference(*preference);
        }
    });
    connect(convertButton_, &QPushButton::clicked, this, [this] { planConversions(); });
    connect(cancelButton_, &QPushButton::clicked, this, [this] { cancelConversions(); });

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, [this] {
        if (themePreference_ == conversor::ThemePreference::System) {
            applyTheme();
        }
    });
#endif

    applyTheme();
}

void MainWindow::updateThemePreference(conversor::ThemePreference preference)
{
    themePreference_ = preference;

    QSettings settings;
    settings.setValue(
        "appearance/theme",
        QString::fromStdString(conversor::themePreferenceKey(themePreference_)));

    applyTheme();
}

void MainWindow::applyTheme()
{
    auto* app = qobject_cast<QApplication*>(QCoreApplication::instance());
    if (app == nullptr) {
        return;
    }

    static const QPalette nativePalette = app->palette();

    const auto systemTheme = detectedSystemTheme();
    if (themePreference_ == conversor::ThemePreference::System && !systemTheme.has_value()) {
        app->setPalette(nativePalette);
        app->setStyleSheet(classicStyleSheet(conversor::EffectiveTheme::Light));
        return;
    }

    const auto effectiveTheme = conversor::resolveEffectiveTheme(themePreference_, systemTheme);
    if (effectiveTheme == conversor::EffectiveTheme::Dark) {
        app->setPalette(darkPalette());
        app->setStyleSheet(classicStyleSheet(effectiveTheme));
        return;
    }

    app->setPalette(lightPalette());
    app->setStyleSheet(classicStyleSheet(effectiveTheme));
}

void MainWindow::configureCategoryTab(conversor::FileCategory category, QWidget* tab)
{
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);
    auto* formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto* formatLabel = new QLabel("Formato de destino", tab);
    auto* combo = new QComboBox(tab);
    auto* warning = new QLabel(tab);
    warning->setWordWrap(true);

    const auto targets = conversor::FormatRegistry::targetFormats(category);
    for (const auto& target : targets) {
        combo->addItem(QString::fromStdString(target.label), QString::fromStdString(target.extension));
    }

    auto updateWarning = [combo, warning, category] {
        const auto extension = combo->currentData().toString().toStdString();
        const auto target = conversor::FormatRegistry::findTarget(category, extension);
        if (target.has_value() && !target->warning.empty()) {
            warning->setText(QString::fromStdString(target->warning));
        } else {
            warning->setText("Opcao prioriza preservacao de qualidade quando o formato permite.");
        }
    };

    connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), tab, updateWarning);

    formLayout->addRow(formatLabel, combo);
    layout->addLayout(formLayout);
    layout->addWidget(warning);

    targetCombos_[category] = combo;
    warningLabels_[category] = warning;
    updateWarning();
}

void MainWindow::addFiles()
{
    const auto paths = QFileDialog::getOpenFileNames(this, "Adicionar arquivos");
    appendFiles(paths);
}

void MainWindow::addFolder()
{
    const auto folder = QFileDialog::getExistingDirectory(this, "Adicionar pasta");
    if (folder.isEmpty()) {
        return;
    }

    QStringList paths;
    QDirIterator iterator(folder, QDir::Files, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        paths.append(iterator.next());
    }

    appendFiles(paths);
}

void MainWindow::appendFiles(const QStringList& paths)
{
    for (const auto& path : paths) {
        appendFile(path);
    }

    refreshTable();
    refreshSummary();
}

void MainWindow::appendFile(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }

    const QFileInfo info(path);
    const auto size = info.exists() ? static_cast<std::uintmax_t>(info.size()) : 0;
    files_.push_back(conversor::FormatRegistry::analyzePath(toPath(path), size));
}

void MainWindow::clearFiles()
{
    files_.clear();
    fileTable_->setRowCount(0);
    progressBar_->setValue(0);
    logView_->clear();
    refreshSummary();
}

void MainWindow::chooseOutputDirectory()
{
    const auto folder = QFileDialog::getExistingDirectory(this, "Pasta de saida", fromPath(outputDirectory_));
    if (folder.isEmpty()) {
        return;
    }

    outputDirectory_ = toPath(folder);
    outputPathEdit_->setText(fromPath(outputDirectory_));
}

void MainWindow::refreshTable()
{
    fileTable_->setRowCount(static_cast<int>(files_.size()));

    for (int row = 0; row < static_cast<int>(files_.size()); ++row) {
        const auto& file = files_[static_cast<std::size_t>(row)];
        const auto status = file.category == conversor::FileCategory::Unknown
            ? "Formato nao suportado"
            : "Pronto";

        fileTable_->setItem(row, 0, new QTableWidgetItem(fromPath(file.path)));
        fileTable_->setItem(row, 1, new QTableWidgetItem(categoryLabel(file.category)));
        fileTable_->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(file.extension)));
        fileTable_->setItem(row, 3, new QTableWidgetItem(QString::number(file.sizeBytes)));
        fileTable_->setItem(row, 4, new QTableWidgetItem(status));
    }
}

void MainWindow::refreshSummary()
{
    int images = 0;
    int audio = 0;
    int video = 0;
    int documents = 0;
    int unknown = 0;

    for (const auto& file : files_) {
        switch (file.category) {
        case conversor::FileCategory::Image:
            ++images;
            break;
        case conversor::FileCategory::Audio:
            ++audio;
            break;
        case conversor::FileCategory::Video:
            ++video;
            break;
        case conversor::FileCategory::Document:
            ++documents;
            break;
        case conversor::FileCategory::Unknown:
            ++unknown;
            break;
        }
    }

    summaryLabel_->setText(QString("%1 arquivo(s): %2 imagem, %3 audio, %4 video, %5 documento, %6 desconhecido")
        .arg(static_cast<qulonglong>(files_.size()))
        .arg(images)
        .arg(audio)
        .arg(video)
        .arg(documents)
        .arg(unknown));
}

QString MainWindow::selectedTarget(conversor::FileCategory category) const
{
    const auto found = targetCombos_.find(category);
    if (found == targetCombos_.end() || found->second == nullptr) {
        return {};
    }

    return found->second->currentData().toString();
}

std::vector<conversor::MediaFile> MainWindow::filesForCategory(conversor::FileCategory category) const
{
    std::vector<conversor::MediaFile> selected;
    for (const auto& file : files_) {
        if (file.category == category) {
            selected.push_back(file);
        }
    }
    return selected;
}

void MainWindow::planConversions()
{
    cancelRequested_ = false;
    convertButton_->setEnabled(false);
    cancelButton_->setEnabled(true);
    progressBar_->setValue(0);

    QDir().mkpath(fromPath(outputDirectory_));
    logView_->appendPlainText("Planejando conversoes...");

    std::vector<conversor::PlannedConversion> allItems;
    for (const auto category : visibleCategories) {
        const auto categoryFiles = filesForCategory(category);
        if (categoryFiles.empty()) {
            continue;
        }

        conversor::ConversionRequest request;
        request.inputs = categoryFiles;
        request.category = category;
        request.targetExtension = selectedTarget(category).toStdString();
        request.outputDirectory = outputDirectory_;

        try {
            const auto plan = conversor::ConversionPlan::build(request);
            for (const auto& item : plan) {
                logView_->appendPlainText(QString("%1 -> %2")
                    .arg(fromPath(item.input.path))
                    .arg(fromPath(item.outputPath)));

                for (const auto& warning : item.warnings) {
                    logView_->appendPlainText(QString("Aviso: %1").arg(QString::fromStdString(warning)));
                }

                allItems.push_back(item);
            }
        } catch (const std::exception& error) {
            logView_->appendPlainText(QString("Erro em %1: %2")
                .arg(categoryLabel(category))
                .arg(error.what()));
        }
    }

    if (allItems.empty()) {
        progressBar_->setValue(0);
        logView_->appendPlainText("Nenhum arquivo compativel para converter.");
        convertButton_->setEnabled(true);
        cancelButton_->setEnabled(false);
        return;
    }

    int completed = 0;
    for (const auto& item : allItems) {
        if (cancelRequested_) {
            logView_->appendPlainText("Conversao cancelada pelo usuario.");
            break;
        }

        const auto command = conversor::EngineCommandBuilder::build(
            item.input.category,
            item.input.path,
            item.outputPath,
            item.target.extension);

        logView_->appendPlainText(QString("Executando motor %1").arg(QString::fromStdString(command.executableKey)));
        executeEngineCommand(command);

        ++completed;
        progressBar_->setValue((completed * 100) / static_cast<int>(allItems.size()));
    }

    activeProcess_ = nullptr;
    convertButton_->setEnabled(true);
    cancelButton_->setEnabled(false);

    if (!cancelRequested_) {
        logView_->appendPlainText(QString("Fila finalizada: %1 arquivo(s) processado(s).").arg(completed));
    }
}

void MainWindow::cancelConversions()
{
    cancelRequested_ = true;
    if (activeProcess_ != nullptr && activeProcess_->state() != QProcess::NotRunning) {
        activeProcess_->kill();
    }
}

QString MainWindow::resolveExecutable(const QString& executableKey) const
{
    const auto appDir = QCoreApplication::applicationDirPath();

    QStringList candidates;
    if (executableKey == "ffmpeg") {
        candidates << appDir + "/engines/ffmpeg/" + executableName("ffmpeg");
    } else if (executableKey == "magick") {
        candidates << appDir + "/engines/imagemagick/" + executableName("magick");
    } else if (executableKey == "soffice") {
        candidates << appDir + "/engines/libreoffice/program/" + executableName("soffice");
        candidates << appDir + "/engines/libreoffice/" + executableName("soffice");
    } else if (executableKey == "pandoc") {
        candidates << appDir + "/engines/pandoc/" + executableName("pandoc");
    }

    for (const auto& candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return QStandardPaths::findExecutable(executableKey);
}

bool MainWindow::executeEngineCommand(const conversor::EngineCommand& command)
{
    const auto executable = resolveExecutable(QString::fromStdString(command.executableKey));
    if (executable.isEmpty()) {
        logView_->appendPlainText(QString("Motor nao encontrado: %1").arg(QString::fromStdString(command.executableKey)));
        return false;
    }

    const auto arguments = toQStringList(command.arguments);
    QProcess process;
    activeProcess_ = &process;
    process.start(executable, arguments);

    if (!process.waitForStarted(5000)) {
        logView_->appendPlainText(QString("Falha ao iniciar motor: %1").arg(executable));
        activeProcess_ = nullptr;
        return false;
    }

    while (process.state() != QProcess::NotRunning) {
        QCoreApplication::processEvents();
        if (cancelRequested_) {
            process.kill();
            process.waitForFinished(3000);
            activeProcess_ = nullptr;
            return false;
        }
        process.waitForFinished(100);
    }

    const auto output = QString::fromLocal8Bit(process.readAllStandardOutput()).trimmed();
    const auto error = QString::fromLocal8Bit(process.readAllStandardError()).trimmed();
    if (!output.isEmpty()) {
        logView_->appendPlainText(output);
    }
    if (!error.isEmpty()) {
        logView_->appendPlainText(error);
    }

    const bool ok = process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
    if (!ok) {
        logView_->appendPlainText(QString("Motor encerrou com codigo %1").arg(process.exitCode()));
    }

    activeProcess_ = nullptr;
    return ok;
}
