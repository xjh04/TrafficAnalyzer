#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QColor>
#include <QFont>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QTabWidget>
#include <QSettings>
#include <QColorDialog>
#include <QFontDialog>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget() override;

    // --- Public Getters to access current settings values ---
    QString getTheme() const;
    QString getLanguage() const;
    QString getDefaultDataSource() const;
    bool isAutoSaveEnabled() const;
    bool isAutoStartEnabled() const;
    bool isMinimizeToTrayEnabled() const;
    int getOpacity() const;
    QColor getCustomThemeColor() const;
    QFont getAppFont() const;
    int getRefreshInterval() const;
    int getMaxRecords() const;
    int getTimeout() const;
    bool isProxyEnabled() const;
    QString getProxyHost() const;
    int getProxyPort() const;
    QString getLogLevel() const;
    bool isAutoExportEnabled() const;
    QString getExportPath() const;
    bool isDebugModeEnabled() const;
    int getBufferSize() const;

public slots:
    // --- Public Setters to programmatically update UI and settings ---
    void setLanguage(const QString &language);
    void setDefaultDataSource(const QString &source);
    void setAutoSave(bool enabled);
    void setAutoStart(bool enabled);
    void setMinimizeToTray(bool enabled);
    void setTheme(const QString &theme);
    void setOpacity(int opacity);
    void setCustomThemeColor(const QColor &color);
    void setAppFont(const QFont &font);
    void setRefreshInterval(int interval);
    void setMaxRecords(int records);
    void setTimeout(int timeout);
    void setProxyEnabled(bool enabled);
    void setProxyHost(const QString &host);
    void setProxyPort(int port);
    void setLogLevel(const QString &level);
    void setAutoExport(bool enabled);
    void setExportPath(const QString &path);
    void setDebugMode(bool enabled);
    void setBufferSize(int size);

    // --- Import/Export functionality ---
    void importSettings();
    void exportSettings();

signals:
    void themeChanged(const QString &theme);
    void languageChanged(const QString &language);
    void settingsChanged();
    void backRequested();

private slots:
    void onThemeChanged();
    void onLanguageChanged();
    void onDefaultDataSourceChanged();
    void onAutoSaveToggled(bool enabled);
    void onRefreshIntervalChanged(int interval);
    void onMaxRecordsChanged(int records);
    void onLogLevelChanged();
    void onAutoExportToggled(bool enabled);
    void onExportPathChanged();
    void onBrowseExportPath();
    void onCustomColorClicked();
    void onFontSettingsClicked();
    void onResetToDefaults();
    void onApplySettings();
    void onCancelSettings();

private:
    void setupUI();
    void setupGeneralTab();
    void setupAppearanceTab();
    void setupNetworkTab();
    void setupAdvancedTab();

    void loadSettings();
    void saveSettings();
    bool validateSettings();

    void updateLanguageUI();
    void applyTheme(const QString &themeName);
    void applyCustomTheme(const QColor &primaryColor);
    void updateCustomColorButton(const QColor &color);

    // UI Components
    QTabWidget *tabWidget;

    // General Settings
    QComboBox *languageCombo;
    QLineEdit *defaultDataSourceEdit;
    QCheckBox *autoSaveCheckBox;
    QCheckBox *autoStartCheckBox;
    QCheckBox *minimizeToTrayCheckBox;

    // Appearance Settings
    QComboBox *themeCombo;
    QSlider *opacitySlider;
    QLabel *opacityValueLabel;
    QPushButton *customColorBtn;
    QPushButton *fontSettingsBtn;
    QLabel *fontPreviewLabel;

    // Network Settings
    QSpinBox *refreshIntervalSpin;
    QSpinBox *maxRecordsSpin;
    QSpinBox *timeoutSpin;
    QLineEdit *proxyHostEdit;
    QSpinBox *proxyPortSpin;
    QCheckBox *proxyEnabledCheckBox;

    // Advanced Settings
    QComboBox *logLevelCombo;
    QCheckBox *autoExportCheckBox;
    QLineEdit *exportPathEdit;
    QPushButton *browseExportBtn;
    QCheckBox *debugModeCheckBox;
    QSpinBox *bufferSizeSpin;

    // Buttons
    QPushButton *resetBtn;
    QPushButton *applyBtn;
    QPushButton *cancelBtn;
    QPushButton *backBtn;

    // Settings Storage
    QSettings *settings;

    // Member variables for settings state
    QColor customThemeColor;
    QFont appFont;
    QString defaultStyleSheet;
};

#endif // SETTINGSWIDGET_H
