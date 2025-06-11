#include "MainWindow.h"
#include "LoginWidget.h"
#include "RegisterWidget.h"
#include "TrafficAnalyzerWidget.h"
#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , stackedWidget(nullptr)
    , loginWidget(nullptr)
    , registerWidget(nullptr)
    , trafficWidget(nullptr)
    , settingsWidget(nullptr)
    , toolBar(nullptr)
    , trayIcon(nullptr)
    , currentTheme("浅色主题")
    , currentLanguage("简体中文")
    , isLoggedIn(false)
{
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupSystemTray();
    loadSettings();
    restoreWindowState();
}

MainWindow::~MainWindow()
{
    saveWindowState();
}

void MainWindow::setupUI()
{
    setWindowTitle("网络流量分析系统 v2.0");
    setFixedSize(1200, 800);
    setWindowIcon(QIcon(":/icons/app_icon.png")); // 如果有图标的话

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // 创建各个界面
    loginWidget = new LoginWidget(this);
    registerWidget = new RegisterWidget(this);
    trafficWidget = new TrafficAnalyzerWidget(this);
    settingsWidget = new SettingsWidget(this);

    // 添加到堆栈窗口
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);
    stackedWidget->addWidget(trafficWidget);
    stackedWidget->addWidget(settingsWidget);

    // 连接登录相关信号槽
    connect(loginWidget, &LoginWidget::registerRequested, this, &MainWindow::showRegister);
    connect(loginWidget, &LoginWidget::loginSuccess, this, &MainWindow::showTrafficAnalyzer);
    connect(registerWidget, &RegisterWidget::backToLogin, this, &MainWindow::showLogin);

    // 连接设置相关信号槽
    connect(settingsWidget, &SettingsWidget::backRequested, this, &MainWindow::showTrafficAnalyzer);
    connect(settingsWidget, &SettingsWidget::settingsChanged, this, &MainWindow::onSettingsChanged);
    connect(settingsWidget, &SettingsWidget::themeChanged, this, &MainWindow::onThemeChanged);
    connect(settingsWidget, &SettingsWidget::languageChanged, this, &MainWindow::onLanguageChanged);

    // 默认显示登录界面
    stackedWidget->setCurrentWidget(loginWidget);

    // 应用默认主题
    applyTheme(currentTheme);
}

void MainWindow::setupMenuBar()
{
    // Fix: Use QMainWindow::menuBar() directly without assignment
    QMenuBar *menuBarPtr = QMainWindow::menuBar();

    // 文件菜单
    fileMenu = menuBarPtr->addMenu("文件(&F)");

    settingsAction = new QAction("设置(&S)", this);
    settingsAction->setShortcut(QKeySequence::Preferences);
    settingsAction->setIcon(QIcon(":/icons/settings.png"));
    settingsAction->setEnabled(false); // 登录后才能使用
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    fileMenu->addAction(settingsAction);

    fileMenu->addSeparator();

    exitAction = new QAction("退出(&X)", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setIcon(QIcon(":/icons/exit.png"));
    connect(exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
    fileMenu->addAction(exitAction);

    // 查看菜单
    viewMenu = menuBarPtr->addMenu("查看(&V)");

    minimizeAction = new QAction("最小化到托盘(&M)", this);
    minimizeAction->setShortcut(QKeySequence("Ctrl+M"));
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);
    viewMenu->addAction(minimizeAction);

    restoreAction = new QAction("恢复窗口(&R)", this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    viewMenu->addAction(restoreAction);

    // 工具菜单
    toolsMenu = menuBarPtr->addMenu("工具(&T)");
    // 可以在这里添加更多工具选项

    // 帮助菜单
    helpMenu = menuBarPtr->addMenu("帮助(&H)");

    aboutAction = new QAction("关于(&A)", this);
    aboutAction->setIcon(QIcon(":/icons/about.png"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupToolBar()
{
    toolBar = addToolBar("主工具栏");
    toolBar->setMovable(false);

    // 添加设置按钮到工具栏
    toolBar->addAction(settingsAction);
    toolBar->addSeparator();

    // 可以添加更多快捷操作按钮
    auto *quickAnalysisAction = new QAction("快速分析", this);
    quickAnalysisAction->setIcon(QIcon(":/icons/analyze.png"));
    quickAnalysisAction->setEnabled(false);
    toolBar->addAction(quickAnalysisAction);

    auto *exportAction = new QAction("导出数据", this);
    exportAction->setIcon(QIcon(":/icons/export.png"));
    exportAction->setEnabled(false);
    toolBar->addAction(exportAction);
}

void MainWindow::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/tray_icon.png"));
    trayIcon->setToolTip("网络流量分析系统");

    // 创建托盘菜单
    trayMenu = new QMenu(this);

    auto *showAction = new QAction("显示主窗口", this);
    connect(showAction, &QAction::triggered, this, [this]() {
        show();
        raise();
        activateWindow();
    });
    trayMenu->addAction(showAction);

    auto *hideAction = new QAction("隐藏窗口", this);
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    trayMenu->addAction(hideAction);

    trayMenu->addSeparator();
    trayMenu->addAction(settingsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

    trayIcon->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon && trayIcon->isVisible()) {
        QMessageBox::information(this, "提示",
                                "程序将最小化到系统托盘。\n要完全退出程序，请右键点击托盘图标选择退出。");
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::showRegister()
{
    stackedWidget->setCurrentWidget(registerWidget);
}

void MainWindow::showLogin()
{
    stackedWidget->setCurrentWidget(loginWidget);
    // Fix: Remove const qualifier from function to allow modification
    isLoggedIn = false;

    // 禁用需要登录的功能
    settingsAction->setEnabled(false);
    if (toolBar) {
        for (auto *action : toolBar->actions()) {
            if (action->text() == "快速分析" || action->text() == "导出数据") {
                action->setEnabled(false);
            }
        }
    }
}

void MainWindow::showTrafficAnalyzer()
{
    stackedWidget->setCurrentWidget(trafficWidget);
    // Fix: Remove const qualifier from function to allow modification
    isLoggedIn = true;

    // 启用需要登录的功能
    settingsAction->setEnabled(true);
    if (toolBar) {
        for (auto *action : toolBar->actions()) {
            if (action->text() == "快速分析" || action->text() == "导出数据") {
                action->setEnabled(true);
            }
        }
    }
}

void MainWindow::showSettings()
{
    if (!isLoggedIn) {
        QMessageBox::warning(this, "提示", "请先登录后再访问设置页面。");
        return;
    }
    stackedWidget->setCurrentWidget(settingsWidget);
}

void MainWindow::onSettingsChanged()
{
    // 设置发生变化时的处理
    QMessageBox::information(this, "提示", "设置已更新，部分更改将在重启后生效。");
}

void MainWindow::onThemeChanged(const QString &theme)
{
    currentTheme = theme;
    applyTheme(theme);
}

void MainWindow::onLanguageChanged(const QString &language)
{
    currentLanguage = language;
    updateLanguage(language);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
        break;
    default:
        break;
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于网络流量分析系统",
                      "<h2>网络流量分析系统 v2.0</h2>"
                      "<p>这是一个功能强大的网络流量分析工具，提供实时监控和数据分析功能。</p>"
                      "<p><b>主要功能：</b></p>"
                      "<ul>"
                      "<li>实时网络流量监控</li>"
                      "<li>多协议支持（TCP、UDP、HTTP等）</li>"
                      "<li>数据可视化和统计分析</li>"
                      "<li>自定义过滤和导出功能</li>"
                      "<li>用户管理和系统设置</li>"
                      "</ul>"
                      "<p><b>开发者：</b> 您的名字</p>"
                      "<p><b>版本：</b> 2.0.0</p>"
                      "<p><b>构建日期：</b> " __DATE__ "</p>");
}

void MainWindow::exitApplication()
{
    int ret = QMessageBox::question(this, "确认退出",
                                   "确定要退出网络流量分析系统吗？",
                                   QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        saveWindowState();
        QApplication::quit();
    }
}

void MainWindow::applyTheme(const QString &theme)
{
    QString styleSheet;

    if (theme == "深色主题") {
        styleSheet = R"(
            QMainWindow {
                background-color: #2b2b2b;
                color: #ffffff;
            }
            QMenuBar {
                background-color: #3c3c3c;
                color: #ffffff;
                border-bottom: 1px solid #555555;
            }
            QMenuBar::item {
                background-color: transparent;
                padding: 4px 8px;
            }
            QMenuBar::item:selected {
                background-color: #555555;
                border: 1px solid #777777;
            }
            QMenu {
                background-color: #3c3c3c;
                color: #ffffff;
                border: 1px solid #555555;
            }
            QMenu::item:selected {
                background-color: #555555;
            }
            QToolBar {
                background-color: #3c3c3c;
                border: none;
                spacing: 2px;
            }
            QToolBar::separator {
                background-color: #555555;
                width: 1px;
                margin: 4px 2px;
            }
        )";
    } else if (theme == "浅色主题") {
        styleSheet = R"(
            QMainWindow {
                background-color: #f8f9fa;
                color: #212529;
            }
            QMenuBar {
                background-color: #ffffff;
                color: #212529;
                border-bottom: 1px solid #dee2e6;
            }
            QMenuBar::item {
                background-color: transparent;
                padding: 4px 8px;
            }
            QMenuBar::item:selected {
                background-color: #e9ecef;
                border: 1px solid #ced4da;
            }
            QMenu {
                background-color: #ffffff;
                color: #212529;
                border: 1px solid #dee2e6;
            }
            QMenu::item:selected {
                background-color: #e9ecef;
            }
            QToolBar {
                background-color: #ffffff;
                border: none;
                spacing: 2px;
            }
            QToolBar::separator {
                background-color: #dee2e6;
                width: 1px;
                margin: 4px 2px;
            }
        )";
    }

    setStyleSheet(styleSheet);
}

void MainWindow::updateLanguage(const QString &language)
{
    // 这里可以实现语言切换逻辑
    // 更新菜单、按钮等文本
    if (language == "English") {
        setWindowTitle("Network Traffic Analyzer v2.0");
        fileMenu->setTitle("File(&F)");
        viewMenu->setTitle("View(&V)");
        toolsMenu->setTitle("Tools(&T)");
        helpMenu->setTitle("Help(&H)");
        settingsAction->setText("Settings(&S)");
        exitAction->setText("Exit(&X)");
        aboutAction->setText("About(&A)");
    } else {
        setWindowTitle("网络流量分析系统 v2.0");
        fileMenu->setTitle("文件(&F)");
        viewMenu->setTitle("查看(&V)");
        toolsMenu->setTitle("工具(&T)");
        helpMenu->setTitle("帮助(&H)");
        settingsAction->setText("设置(&S)");
        exitAction->setText("退出(&X)");
        aboutAction->setText("关于(&A)");
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("NetworkAnalyzer", "MainWindow");

    currentTheme = settings.value("theme", "浅色主题").toString();
    currentLanguage = settings.value("language", "简体中文").toString();

    applyTheme(currentTheme);
    updateLanguage(currentLanguage);
}

void MainWindow::saveWindowState()
{
    QSettings settings("NetworkAnalyzer", "MainWindow");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("theme", currentTheme);
    settings.setValue("language", currentLanguage);
}

void MainWindow::restoreWindowState()
{
    QSettings settings("NetworkAnalyzer", "MainWindow");

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}
