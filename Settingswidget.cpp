#include "SettingsWidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QColorDialog>
#include <QFontDialog>
#include <QRegularExpression>
#include <QFileInfo>
#include <QStyle>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , settings(new QSettings("NetworkAnalyzer", "Settings", this))
    , customThemeColor(Qt::blue)
    , appFont(QApplication::font())
{
    setupUI();
    defaultStyleSheet = styleSheet();
    loadSettings();
}

SettingsWidget::~SettingsWidget() = default;

void SettingsWidget::setupUI()
{
    setWindowTitle("系统设置");
    setStyleSheet(R"(
        QWidget {
            background-color: #f8f9fa;
            font-family: 'Microsoft YaHei', Arial, sans-serif;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #dee2e6;
            border-radius: 8px;
            margin: 10px 0px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: #495057;
        }
        QTabWidget::pane {
            border: 1px solid #dee2e6;
            border-radius: 8px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: #e9ecef;
            border: 1px solid #dee2e6;
            padding: 8px 20px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
        }
        QPushButton {
            background-color: #007bff;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0056b3;
        }
        QPushButton:pressed {
            background-color: #004085;
        }
        QComboBox, QLineEdit, QSpinBox {
            padding: 6px 10px;
            border: 1px solid #ced4da;
            border-radius: 4px;
            background-color: white;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus {
            border-color: #007bff;
            outline: none;
        }
    )");

    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *titleLabel = new QLabel("系统设置");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin: 20px 0;");

    // 返回按钮
    backBtn = new QPushButton("← 返回主界面");
    backBtn->setObjectName("backBtn");
    backBtn->setStyleSheet("QPushButton { background-color: #6c757d; margin: 10px 0; }");
    connect(backBtn, &QPushButton::clicked, this, &SettingsWidget::backRequested);

    // 标签页
    tabWidget = new QTabWidget();
    setupGeneralTab();
    setupAppearanceTab();
    setupNetworkTab();
    setupAdvancedTab();

    // 底部按钮
    auto *buttonLayout = new QHBoxLayout();

    resetBtn = new QPushButton("恢复默认");
    resetBtn->setObjectName("resetBtn");
    resetBtn->setStyleSheet("QPushButton { background-color: #ffc107; color: #212529; }");

    applyBtn = new QPushButton("应用设置");
    applyBtn->setObjectName("applyBtn");
    applyBtn->setStyleSheet("QPushButton { background-color: #28a745; }");

    cancelBtn = new QPushButton("取消");
    cancelBtn->setObjectName("cancelBtn");
    cancelBtn->setStyleSheet("QPushButton { background-color: #6c757d; }");

    buttonLayout->addWidget(resetBtn);
    buttonLayout->addStretch();
    buttonLayout->addWidget(applyBtn);
    buttonLayout->addWidget(cancelBtn);

    // 连接信号槽
    connect(resetBtn, &QPushButton::clicked, this, &SettingsWidget::onResetToDefaults);
    connect(applyBtn, &QPushButton::clicked, this, &SettingsWidget::onApplySettings);
    connect(cancelBtn, &QPushButton::clicked, this, &SettingsWidget::onCancelSettings);

    // 添加到主布局
    mainLayout->addWidget(backBtn);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(tabWidget, 1);
    mainLayout->addLayout(buttonLayout);
}

void SettingsWidget::setupGeneralTab()
{
    auto *generalTab = new QWidget();
    auto *layout = new QVBoxLayout(generalTab);

    // 语言设置组
    auto *langGroup = new QGroupBox("语言设置");
    auto *langLayout = new QGridLayout(langGroup);

    langLayout->addWidget(new QLabel("界面语言:"), 0, 0);
    languageCombo = new QComboBox();
    languageCombo->addItems({"简体中文", "English", "繁體中文", "日本語"});
    langLayout->addWidget(languageCombo, 0, 1);
    connect(languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onLanguageChanged);

    // 数据源设置组
    auto *sourceGroup = new QGroupBox("默认数据源");
    auto *sourceLayout = new QGridLayout(sourceGroup);

    sourceLayout->addWidget(new QLabel("默认数据源:"), 0, 0);
    defaultDataSourceEdit = new QLineEdit();
    defaultDataSourceEdit->setPlaceholderText("例如: 192.168.1.1 或 eth0");
    sourceLayout->addWidget(defaultDataSourceEdit, 0, 1);
    connect(defaultDataSourceEdit, &QLineEdit::textChanged,
            this, &SettingsWidget::onDefaultDataSourceChanged);

    // 启动设置组
    auto *startupGroup = new QGroupBox("启动设置");
    auto *startupLayout = new QVBoxLayout(startupGroup);

    autoSaveCheckBox = new QCheckBox("自动保存分析结果");
    autoStartCheckBox = new QCheckBox("程序启动时自动开始分析");
    minimizeToTrayCheckBox = new QCheckBox("最小化到系统托盘");

    startupLayout->addWidget(autoSaveCheckBox);
    startupLayout->addWidget(autoStartCheckBox);
    startupLayout->addWidget(minimizeToTrayCheckBox);

    connect(autoSaveCheckBox, &QCheckBox::toggled, this, &SettingsWidget::onAutoSaveToggled);

    layout->addWidget(langGroup);
    layout->addWidget(sourceGroup);
    layout->addWidget(startupGroup);
    layout->addStretch();

    tabWidget->addTab(generalTab, "常规");
}

void SettingsWidget::setupAppearanceTab()
{
    auto *appearanceTab = new QWidget();
    auto *layout = new QVBoxLayout(appearanceTab);

    // 主题设置组
    auto *themeGroup = new QGroupBox("主题设置");
    auto *themeLayout = new QGridLayout(themeGroup);

    themeLayout->addWidget(new QLabel("主题:"), 0, 0);
    themeCombo = new QComboBox();
    themeCombo->addItems({"浅色主题", "深色主题", "自动跟随系统", "自定义"});
    themeLayout->addWidget(themeCombo, 0, 1);
    connect(themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onThemeChanged);

    customColorBtn = new QPushButton("自定义颜色");
    customColorBtn->setEnabled(false);
    themeLayout->addWidget(customColorBtn, 0, 2);
    connect(customColorBtn, &QPushButton::clicked, this, &SettingsWidget::onCustomColorClicked);

    themeLayout->addWidget(new QLabel("窗口透明度:"), 1, 0);
    opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setRange(50, 100);
    opacitySlider->setValue(100);
    themeLayout->addWidget(opacitySlider, 1, 1);

    opacityValueLabel = new QLabel("100%");
    themeLayout->addWidget(opacityValueLabel, 1, 2);

    connect(opacitySlider, &QSlider::valueChanged, [this](int value) {
        opacityValueLabel->setText(QString("%1%").arg(value));
        if (parentWidget()) {
            parentWidget()->setWindowOpacity(value / 100.0);
        }
    });

    // 字体设置组
    auto *fontGroup = new QGroupBox("字体设置");
    auto *fontLayout = new QGridLayout(fontGroup);

    fontSettingsBtn = new QPushButton("选择字体");
    fontLayout->addWidget(fontSettingsBtn, 0, 0);
    connect(fontSettingsBtn, &QPushButton::clicked, this, &SettingsWidget::onFontSettingsClicked);

    fontPreviewLabel = new QLabel("字体预览: 网络流量分析系统 ABC 123");
    fontPreviewLabel->setStyleSheet("border: 1px solid #ccc; padding: 10px; background-color: white;");
    fontLayout->addWidget(fontPreviewLabel, 1, 0, 1, 2);

    layout->addWidget(themeGroup);
    layout->addWidget(fontGroup);
    layout->addStretch();

    tabWidget->addTab(appearanceTab, "外观");
}

void SettingsWidget::setupNetworkTab()
{
    auto *networkTab = new QWidget();
    auto *layout = new QVBoxLayout(networkTab);

    // 监控设置组
    auto *monitorGroup = new QGroupBox("监控设置");
    auto *monitorLayout = new QGridLayout(monitorGroup);

    monitorLayout->addWidget(new QLabel("刷新间隔 (秒):"), 0, 0);
    refreshIntervalSpin = new QSpinBox();
    refreshIntervalSpin->setRange(1, 60);
    refreshIntervalSpin->setValue(5);
    refreshIntervalSpin->setSuffix(" 秒");
    monitorLayout->addWidget(refreshIntervalSpin, 0, 1);
    connect(refreshIntervalSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsWidget::onRefreshIntervalChanged);

    monitorLayout->addWidget(new QLabel("最大记录数:"), 1, 0);
    maxRecordsSpin = new QSpinBox();
    maxRecordsSpin->setRange(100, 10000);
    maxRecordsSpin->setValue(1000);
    maxRecordsSpin->setSuffix(" 条");
    monitorLayout->addWidget(maxRecordsSpin, 1, 1);
    connect(maxRecordsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &SettingsWidget::onMaxRecordsChanged);

    monitorLayout->addWidget(new QLabel("连接超时 (秒):"), 2, 0);
    timeoutSpin = new QSpinBox();
    timeoutSpin->setRange(5, 120);
    timeoutSpin->setValue(30);
    timeoutSpin->setSuffix(" 秒");
    monitorLayout->addWidget(timeoutSpin, 2, 1);

    // 代理设置组
    auto *proxyGroup = new QGroupBox("代理设置");
    auto *proxyLayout = new QGridLayout(proxyGroup);

    proxyEnabledCheckBox = new QCheckBox("启用代理");
    proxyLayout->addWidget(proxyEnabledCheckBox, 0, 0, 1, 2);

    proxyLayout->addWidget(new QLabel("代理主机:"), 1, 0);
    proxyHostEdit = new QLineEdit();
    proxyHostEdit->setPlaceholderText("例如: proxy.example.com");
    proxyHostEdit->setEnabled(false);
    proxyLayout->addWidget(proxyHostEdit, 1, 1);

    proxyLayout->addWidget(new QLabel("代理端口:"), 2, 0);
    proxyPortSpin = new QSpinBox();
    proxyPortSpin->setRange(1, 65535);
    proxyPortSpin->setValue(8080);
    proxyPortSpin->setEnabled(false);
    proxyLayout->addWidget(proxyPortSpin, 2, 1);

    connect(proxyEnabledCheckBox, &QCheckBox::toggled, [this](bool enabled) {
        proxyHostEdit->setEnabled(enabled);
        proxyPortSpin->setEnabled(enabled);
    });

    layout->addWidget(monitorGroup);
    layout->addWidget(proxyGroup);
    layout->addStretch();

    tabWidget->addTab(networkTab, "网络");
}

void SettingsWidget::setupAdvancedTab()
{
    auto *advancedTab = new QWidget();
    auto *layout = new QVBoxLayout(advancedTab);

    // 日志设置组
    auto *logGroup = new QGroupBox("日志设置");
    auto *logLayout = new QGridLayout(logGroup);

    logLayout->addWidget(new QLabel("日志级别:"), 0, 0);
    logLevelCombo = new QComboBox();
    logLevelCombo->addItems({"调试", "信息", "警告", "错误"});
    logLevelCombo->setCurrentText("信息");
    logLayout->addWidget(logLevelCombo, 0, 1);
    connect(logLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onLogLevelChanged);

    // 导出设置组
    auto *exportGroup = new QGroupBox("导出设置");
    auto *exportLayout = new QGridLayout(exportGroup);

    autoExportCheckBox = new QCheckBox("自动导出分析结果");
    exportLayout->addWidget(autoExportCheckBox, 0, 0, 1, 3);
    connect(autoExportCheckBox, &QCheckBox::toggled, this, &SettingsWidget::onAutoExportToggled);

    exportLayout->addWidget(new QLabel("导出路径:"), 1, 0);
    exportPathEdit = new QLineEdit();
    exportPathEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    exportPathEdit->setEnabled(false);
    exportLayout->addWidget(exportPathEdit, 1, 1);
    connect(exportPathEdit, &QLineEdit::textChanged, this, &SettingsWidget::onExportPathChanged);

    browseExportBtn = new QPushButton("浏览");
    browseExportBtn->setEnabled(false);
    exportLayout->addWidget(browseExportBtn, 1, 2);
    connect(browseExportBtn, &QPushButton::clicked, this, &SettingsWidget::onBrowseExportPath);

    // 高级选项组
    auto *advancedGroup = new QGroupBox("高级选项");
    auto *advancedLayout = new QGridLayout(advancedGroup);

    debugModeCheckBox = new QCheckBox("启用调试模式");
    advancedLayout->addWidget(debugModeCheckBox, 0, 0);

    advancedLayout->addWidget(new QLabel("缓冲区大小 (KB):"), 1, 0);
    bufferSizeSpin = new QSpinBox();
    bufferSizeSpin->setRange(64, 4096);
    bufferSizeSpin->setValue(512);
    bufferSizeSpin->setSuffix(" KB");
    advancedLayout->addWidget(bufferSizeSpin, 1, 1);

    // 导入/导出设置组
    auto *importExportGroup = new QGroupBox("导入/导出设置");
    auto *importExportLayout = new QHBoxLayout(importExportGroup);

    auto *importBtn = new QPushButton("导入设置...");
    auto *exportBtn = new QPushButton("导出设置...");

    importExportLayout->addWidget(importBtn);
    importExportLayout->addWidget(exportBtn);
    importExportLayout->addStretch();

    connect(importBtn, &QPushButton::clicked, this, &SettingsWidget::importSettings);
    connect(exportBtn, &QPushButton::clicked, this, &SettingsWidget::exportSettings);

    layout->addWidget(logGroup);
    layout->addWidget(exportGroup);
    layout->addWidget(advancedGroup);
    layout->addWidget(importExportGroup);
    layout->addStretch();

    tabWidget->addTab(advancedTab, "高级");
}

void SettingsWidget::loadSettings()
{
    settings->sync(); // Ensure we are reading the latest settings
    // 加载常规设置
    languageCombo->setCurrentText(settings->value("language", "简体中文").toString());
    defaultDataSourceEdit->setText(settings->value("defaultDataSource", "").toString());
    autoSaveCheckBox->setChecked(settings->value("autoSave", true).toBool());
    autoStartCheckBox->setChecked(settings->value("autoStart", false).toBool());
    minimizeToTrayCheckBox->setChecked(settings->value("minimizeToTray", false).toBool());

    // 加载外观设置
    themeCombo->setCurrentText(settings->value("theme", "浅色主题").toString());
    opacitySlider->setValue(settings->value("opacity", 100).toInt());
    customThemeColor = QColor(settings->value("customThemeColor", "#007bff").toString());
    appFont = settings->value("font", QApplication::font()).value<QFont>();
    setAppFont(appFont); // Apply font to widget and preview

    // 加载网络设置
    refreshIntervalSpin->setValue(settings->value("refreshInterval", 5).toInt());
    maxRecordsSpin->setValue(settings->value("maxRecords", 1000).toInt());
    timeoutSpin->setValue(settings->value("timeout", 30).toInt());
    proxyEnabledCheckBox->setChecked(settings->value("proxyEnabled", false).toBool());
    proxyHostEdit->setText(settings->value("proxyHost", "").toString());
    proxyPortSpin->setValue(settings->value("proxyPort", 8080).toInt());

    // 加载高级设置
    logLevelCombo->setCurrentText(settings->value("logLevel", "信息").toString());
    autoExportCheckBox->setChecked(settings->value("autoExport", false).toBool());
    exportPathEdit->setText(settings->value("exportPath",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString());
    debugModeCheckBox->setChecked(settings->value("debugMode", false).toBool());
    bufferSizeSpin->setValue(settings->value("bufferSize", 512).toInt());

    // 应用加载的设置到UI
    applyTheme(themeCombo->currentText());
    if (themeCombo->currentText() == "自定义") {
        updateCustomColorButton(customThemeColor);
    }
    updateLanguageUI();
}

void SettingsWidget::saveSettings()
{
    // 保存常规设置
    settings->setValue("language", languageCombo->currentText());
    settings->setValue("defaultDataSource", defaultDataSourceEdit->text());
    settings->setValue("autoSave", autoSaveCheckBox->isChecked());
    settings->setValue("autoStart", autoStartCheckBox->isChecked());
    settings->setValue("minimizeToTray", minimizeToTrayCheckBox->isChecked());

    // 保存外观设置
    settings->setValue("theme", themeCombo->currentText());
    settings->setValue("opacity", opacitySlider->value());
    settings->setValue("customThemeColor", customThemeColor.name());
    settings->setValue("font", appFont);

    // 保存网络设置
    settings->setValue("refreshInterval", refreshIntervalSpin->value());
    settings->setValue("maxRecords", maxRecordsSpin->value());
    settings->setValue("timeout", timeoutSpin->value());
    settings->setValue("proxyEnabled", proxyEnabledCheckBox->isChecked());
    settings->setValue("proxyHost", proxyHostEdit->text());
    settings->setValue("proxyPort", proxyPortSpin->value());

    // 保存高级设置
    settings->setValue("logLevel", logLevelCombo->currentText());
    settings->setValue("autoExport", autoExportCheckBox->isChecked());
    settings->setValue("exportPath", exportPathEdit->text());
    settings->setValue("debugMode", debugModeCheckBox->isChecked());
    settings->setValue("bufferSize", bufferSizeSpin->value());

    settings->sync();
}

// --- Slot Implementations ---

void SettingsWidget::onThemeChanged()
{
    const QString theme = themeCombo->currentText();
    customColorBtn->setEnabled(theme == "自定义");
    applyTheme(theme);
    if (theme == "自定义") {
        updateCustomColorButton(customThemeColor);
    }
    emit themeChanged(theme);
}

void SettingsWidget::onLanguageChanged()
{
    updateLanguageUI();
    emit languageChanged(languageCombo->currentText());
}

void SettingsWidget::onDefaultDataSourceChanged() { /* Validation logic can be added here */ }
void SettingsWidget::onAutoSaveToggled(bool enabled) { Q_UNUSED(enabled) }
void SettingsWidget::onRefreshIntervalChanged(int interval) { Q_UNUSED(interval) }
void SettingsWidget::onMaxRecordsChanged(int records) { Q_UNUSED(records) }
void SettingsWidget::onLogLevelChanged() { /* Logging level change logic can be added here */ }
void SettingsWidget::onAutoExportToggled(bool enabled)
{
    exportPathEdit->setEnabled(enabled);
    browseExportBtn->setEnabled(enabled);
}

void SettingsWidget::onExportPathChanged() { /* Path validation logic can be added here */ }

void SettingsWidget::onBrowseExportPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择导出目录", exportPathEdit->text());
    if (!dir.isEmpty()) {
        exportPathEdit->setText(dir);
    }
}

void SettingsWidget::onCustomColorClicked()
{
    QColor color = QColorDialog::getColor(customThemeColor, this, "选择自定义颜色");
    if (color.isValid()) {
        setCustomThemeColor(color);
    }
}

void SettingsWidget::onFontSettingsClicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this->appFont, this, "选择字体");
    if (ok) {
        setAppFont(font);
    }
}

void SettingsWidget::onResetToDefaults()
{
    int ret = QMessageBox::question(this, "确认", "确定要恢复所有设置到默认值吗？",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        settings->clear();
        customThemeColor = QColor("#007bff"); // Reset custom color
        appFont = QApplication::font(); // Reset font
        loadSettings();
        QMessageBox::information(this, "完成", "设置已恢复到默认值");
    }
}

void SettingsWidget::onApplySettings()
{
    if (!validateSettings()) {
        return; // Stop if validation fails
    }
    saveSettings();
    emit settingsChanged();
    QMessageBox::information(this, "完成", "设置已保存");
}

void SettingsWidget::onCancelSettings()
{
    loadSettings(); // Re-loads saved settings, discarding any changes
}

// --- Helper Functions ---

void SettingsWidget::applyTheme(const QString &themeName)
{
    if (themeName == "自定义") {
        applyCustomTheme(customThemeColor);
    } else if (themeName == "深色主题") {
        QString darkStyle = R"(
            QWidget { background-color: #2c3e50; color: #ecf0f1; font-family: 'Microsoft YaHei', Arial, sans-serif; }
            QGroupBox { border: 2px solid #34495e; }
            QGroupBox::title { color: #1abc9c; }
            QTabWidget::pane { border: 1px solid #34495e; background-color: #34495e; }
            QTabBar::tab { background-color: #2c3e50; border: 1px solid #34495e; }
            QTabBar::tab:selected { background-color: #34495e; }
            QPushButton { background-color: #3498db; color: white; border: none; padding: 8px 16px; border-radius: 6px; font-weight: bold; }
            QPushButton:hover { background-color: #2980b9; }
            QPushButton:pressed { background-color: #1f618d; }
            QComboBox, QLineEdit, QSpinBox { border: 1px solid #7f8c8d; background-color: #5d6d7e; color: #ecf0f1; }
            QComboBox:focus, QLineEdit:focus, QSpinBox:focus { border-color: #3498db; }
            QPushButton#resetBtn { background-color: #f1c40f; color: #2c3e50; }
            QPushButton#applyBtn { background-color: #2ecc71; }
            QPushButton#cancelBtn, QPushButton#backBtn { background-color: #95a5a6; }
        )";
        setStyleSheet(darkStyle);
    } else { // 浅色主题 or 自动跟随系统
        setStyleSheet(defaultStyleSheet);
        // Re-apply specific styles that might have been lost
        backBtn->setStyleSheet("QPushButton { background-color: #6c757d; margin: 10px 0; }");
        resetBtn->setStyleSheet("QPushButton { background-color: #ffc107; color: #212529; }");
        applyBtn->setStyleSheet("QPushButton { background-color: #28a745; }");
        cancelBtn->setStyleSheet("QPushButton { background-color: #6c757d; }");
    }
    // Ensure font and styles are correctly applied after stylesheet change
    setFont(appFont);
    style()->unpolish(this);
    style()->polish(this);
}

void SettingsWidget::updateLanguageUI()
{
    // This function should be expanded with full translations
    QString lang = languageCombo->currentText();
    if (lang == "English") {
        setWindowTitle("System Settings");
        // ... set text for all other widgets ...
    } else if (lang == "繁體中文") {
        setWindowTitle("系統設定");
        // ... set text for all other widgets ...
    } else { // 简体中文 (default)
        setWindowTitle("系统设置");
        // ... (already in Chinese) ...
    }
}


void SettingsWidget::applyCustomTheme(const QColor &primaryColor)
{
    // Generate a color scheme based on the primary color
    QColor lightColor = primaryColor.lighter(150);
    QColor darkColor = primaryColor.darker(150);
    QColor hoverColor = primaryColor.darker(110);
    QColor pressedColor = primaryColor.darker(130);

    QString customThemeStyle = QString(R"(
        QWidget {
            background-color: #f8f9fa;
            font-family: 'Microsoft YaHei', Arial, sans-serif;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid %1;
            border-radius: 8px;
            margin: 10px 0px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 10px 0 10px;
            color: %2;
        }
        QTabWidget::pane {
            border: 1px solid %1;
            border-radius: 8px;
            background-color: white;
        }
        QTabBar::tab {
            background-color: %3;
            border: 1px solid %1;
            padding: 8px 20px;
            margin-right: 2px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }
        QTabBar::tab:selected {
            background-color: white;
            border-bottom-color: white;
        }
        QPushButton {
            background-color: %4;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 6px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: %5;
        }
        QPushButton:pressed {
            background-color: %6;
        }
        QComboBox, QLineEdit, QSpinBox {
            padding: 6px 10px;
            border: 1px solid %1;
            border-radius: 4px;
            background-color: white;
        }
        QComboBox:focus, QLineEdit:focus, QSpinBox:focus {
            border-color: %4;
            outline: none;
        }
        QSlider::groove:horizontal {
            border: 1px solid %1; height: 8px; background: #f0f0f0; border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: %4; border: 1px solid %2; width: 18px; margin: -5px 0; border-radius: 9px;
        }
        QCheckBox::indicator:checked {
            background-color: %4; border: 1px solid %2;
        }
    )")
    .arg(lightColor.name())        // %1 - border color
    .arg(darkColor.name())         // %2 - dark text and borders
    .arg(lightColor.lighter(120).name()) // %3 - tab background
    .arg(primaryColor.name())      // %4 - primary color
    .arg(hoverColor.name())        // %5 - hover color
    .arg(pressedColor.name());     // %6 - pressed color

    setStyleSheet(customThemeStyle);
}

void SettingsWidget::updateCustomColorButton(const QColor &color)
{
    QString buttonStyle = QString(
        "QPushButton { "
        "background-color: %1; "
        "color: %2; "
        "border: 2px solid %3; "
        "padding: 8px 16px; "
        "border-radius: 6px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { background-color: %4; } "
        "QPushButton:pressed { background-color: %5; }"
    ).arg(color.name())
     .arg(color.lightness() > 128 ? "#000000" : "#ffffff") // Choose text color based on brightness
     .arg(color.darker(120).name())
     .arg(color.lighter(110).name())
     .arg(color.darker(110).name());

    customColorBtn->setStyleSheet(buttonStyle);
    customColorBtn->setText(QString("自定义颜色 (%1)").arg(color.name().toUpper()));
}

// --- Getter functions for accessing settings values ---

QString SettingsWidget::getLanguage() const { return languageCombo->currentText(); }
QString SettingsWidget::getDefaultDataSource() const { return defaultDataSourceEdit->text(); }
bool SettingsWidget::isAutoSaveEnabled() const { return autoSaveCheckBox->isChecked(); }
bool SettingsWidget::isAutoStartEnabled() const { return autoStartCheckBox->isChecked(); }
bool SettingsWidget::isMinimizeToTrayEnabled() const { return minimizeToTrayCheckBox->isChecked(); }
QString SettingsWidget::getTheme() const { return themeCombo->currentText(); }
int SettingsWidget::getOpacity() const { return opacitySlider->value(); }
QColor SettingsWidget::getCustomThemeColor() const { return customThemeColor; }
QFont SettingsWidget::getAppFont() const { return appFont; }
int SettingsWidget::getRefreshInterval() const { return refreshIntervalSpin->value(); }
int SettingsWidget::getMaxRecords() const { return maxRecordsSpin->value(); }
int SettingsWidget::getTimeout() const { return timeoutSpin->value(); }
bool SettingsWidget::isProxyEnabled() const { return proxyEnabledCheckBox->isChecked(); }
QString SettingsWidget::getProxyHost() const { return proxyHostEdit->text(); }
int SettingsWidget::getProxyPort() const { return proxyPortSpin->value(); }
QString SettingsWidget::getLogLevel() const { return logLevelCombo->currentText(); }
bool SettingsWidget::isAutoExportEnabled() const { return autoExportCheckBox->isChecked(); }
QString SettingsWidget::getExportPath() const { return exportPathEdit->text(); }
bool SettingsWidget::isDebugModeEnabled() const { return debugModeCheckBox->isChecked(); }
int SettingsWidget::getBufferSize() const { return bufferSizeSpin->value(); }

// --- Setter functions for programmatically updating settings ---

void SettingsWidget::setLanguage(const QString &language) { languageCombo->setCurrentText(language); }
void SettingsWidget::setDefaultDataSource(const QString &source) { defaultDataSourceEdit->setText(source); }
void SettingsWidget::setAutoSave(bool enabled) { autoSaveCheckBox->setChecked(enabled); }
void SettingsWidget::setAutoStart(bool enabled) { autoStartCheckBox->setChecked(enabled); }
void SettingsWidget::setMinimizeToTray(bool enabled) { minimizeToTrayCheckBox->setChecked(enabled); }
void SettingsWidget::setTheme(const QString &theme) { themeCombo->setCurrentText(theme); }
void SettingsWidget::setOpacity(int opacity) { opacitySlider->setValue(opacity); }
void SettingsWidget::setCustomThemeColor(const QColor &color)
{
    customThemeColor = color;
    if (themeCombo->currentText() == "自定义") {
        applyCustomTheme(color);
        updateCustomColorButton(color);
    }
}
void SettingsWidget::setAppFont(const QFont &font)
{
    appFont = font;
    setFont(font);
    fontPreviewLabel->setFont(font);
    fontPreviewLabel->setText(QString("字体预览: %1 %2pt").arg(font.family()).arg(font.pointSize()));
}
void SettingsWidget::setRefreshInterval(int interval) { refreshIntervalSpin->setValue(interval); }
void SettingsWidget::setMaxRecords(int records) { maxRecordsSpin->setValue(records); }
void SettingsWidget::setTimeout(int timeout) { timeoutSpin->setValue(timeout); }
void SettingsWidget::setProxyEnabled(bool enabled) { proxyEnabledCheckBox->setChecked(enabled); }
void SettingsWidget::setProxyHost(const QString &host) { proxyHostEdit->setText(host); }
void SettingsWidget::setProxyPort(int port) { proxyPortSpin->setValue(port); }
void SettingsWidget::setLogLevel(const QString &level) { logLevelCombo->setCurrentText(level); }
void SettingsWidget::setAutoExport(bool enabled) { autoExportCheckBox->setChecked(enabled); }
void SettingsWidget::setExportPath(const QString &path) { exportPathEdit->setText(path); }
void SettingsWidget::setDebugMode(bool enabled) { debugModeCheckBox->setChecked(enabled); }
void SettingsWidget::setBufferSize(int size) { bufferSizeSpin->setValue(size); }

// --- Utility Functions ---
bool SettingsWidget::validateSettings()
{
    if (isProxyEnabled() && getProxyHost().isEmpty()) {
        QMessageBox::warning(this, "设置错误", "启用代理时必须填写代理主机地址。");
        return false;
    }

    if (isAutoExportEnabled()) {
        QString path = getExportPath();
        QFileInfo dirInfo(path);
        if (!dirInfo.exists() || !dirInfo.isDir()) {
            QMessageBox::warning(this, "设置错误", QString("导出路径不是一个有效的目录: %1").arg(path));
            return false;
        }
        if (!dirInfo.isWritable()) {
            QMessageBox::warning(this, "设置错误", QString("导出路径没有写入权限: %1").arg(path));
            return false;
        }
    }

    QString dataSource = getDefaultDataSource();
    if (!dataSource.isEmpty()) {
        QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
        QRegularExpression interfaceRegex("^[a-zA-Z0-9_.-]+$");
        if (!ipRegex.match(dataSource).hasMatch() && !interfaceRegex.match(dataSource).hasMatch()) {
            int ret = QMessageBox::question(this, "数据源格式警告",
                "默认数据源格式可能不正确，应为IP地址或网络接口名称。\n是否仍然继续保存？",
                QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::No) {
                return false;
            }
        }
    }

    return true;
}

void SettingsWidget::exportSettings()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出设置",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/network_analyzer_settings.ini",
        "配置文件 (*.ini)");

    if (!fileName.isEmpty()) {
        saveSettings(); // Ensure current UI values are saved before exporting
        QSettings exportSettings(fileName, QSettings::IniFormat);
        for (const QString &key : settings->allKeys()) {
            exportSettings.setValue(key, settings->value(key));
        }
        exportSettings.sync();
        QMessageBox::information(this, "导出完成", QString("设置已成功导出到:\n%1").arg(fileName));
    }
}

void SettingsWidget::importSettings()
{
    QString fileName = QFileDialog::getOpenFileName(this, "导入设置",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "配置文件 (*.ini)");

    if (!fileName.isEmpty()) {
        int ret = QMessageBox::question(this, "确认导入",
            "导入设置将覆盖当前所有设置并立即应用。是否继续？", QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            QSettings importSettings(fileName, QSettings::IniFormat);
            settings->clear();
            for (const QString &key : importSettings.allKeys()) {
                settings->setValue(key, importSettings.value(key));
            }
            settings->sync();
            loadSettings(); // Reload UI with imported settings
            emit settingsChanged(); // Notify the application of changes
            QMessageBox::information(this, "导入完成", "设置已成功导入。");
        }
    }
}
