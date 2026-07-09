#pragma once

#include "core/EngineCommand.h"
#include "core/MediaFile.h"
#include "core/ThemePreference.h"

#include <QMainWindow>

#include <filesystem>
#include <map>
#include <vector>

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QProgressBar;
class QProcess;
class QPushButton;
class QTableWidget;
class QTabWidget;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void addFiles();
    void addFolder();
    void clearFiles();
    void chooseOutputDirectory();
    void planConversions();
    void cancelConversions();
    void updateThemePreference(conversor::ThemePreference preference);
    void applyTheme();
    void refreshTable();
    void refreshSummary();
    void appendFile(const QString& path);
    void appendFiles(const QStringList& paths);
    void configureCategoryTab(conversor::FileCategory category, QWidget* tab);
    bool executeEngineCommand(const conversor::EngineCommand& command);
    QString resolveExecutable(const QString& executableKey) const;
    QString selectedTarget(conversor::FileCategory category) const;
    std::vector<conversor::MediaFile> filesForCategory(conversor::FileCategory category) const;

    QTableWidget* fileTable_ = nullptr;
    QLabel* summaryLabel_ = nullptr;
    QLabel* outputLabel_ = nullptr;
    QTabWidget* categoryTabs_ = nullptr;
    QComboBox* themeCombo_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
    QPushButton* convertButton_ = nullptr;
    QPushButton* cancelButton_ = nullptr;
    QPlainTextEdit* logView_ = nullptr;

    std::filesystem::path outputDirectory_;
    std::vector<conversor::MediaFile> files_;
    std::map<conversor::FileCategory, QComboBox*> targetCombos_;
    std::map<conversor::FileCategory, QLabel*> warningLabels_;
    conversor::ThemePreference themePreference_ = conversor::ThemePreference::System;
    QProcess* activeProcess_ = nullptr;
    bool cancelRequested_ = false;
};
