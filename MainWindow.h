#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QSystemTrayIcon>
#include <QString>

// 前向声明
class LoginWidget;
class RegisterWidget;
class TrafficAnalyzerWidget;
class SettingsWidget;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 页面切换槽函数
    void showRegister();
    void showLogin();
    void showTrafficAnalyzer();
    void showSettings();

    // 设置相关槽函数
    void onSettingsChanged();
    void onThemeChanged(const QString &theme);
    void onLanguageChanged(const QString &language);

    // 系统托盘槽函数
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    // 菜单动作槽函数
    void showAbout();
    void exitApplication();

private:
    // UI初始化函数
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupSystemTray();

    // 主题和语言相关函数
    void applyTheme(const QString &theme);
    void updateLanguage(const QString &language);

    // 设置保存和恢复
    void loadSettings();
    void saveWindowState();
    void restoreWindowState();

    // 主要UI组件
    QStackedWidget *stackedWidget;
    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;
    TrafficAnalyzerWidget *trafficWidget;
    SettingsWidget *settingsWidget;

    // 菜单和工具栏
    QToolBar *toolBar;

    // 菜单
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;

    // 动作
    QAction *settingsAction;
    QAction *exitAction;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *aboutAction;

    // 系统托盘
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;

    // 状态变量
    QString currentTheme;
    QString currentLanguage;
    bool isLoggedIn;
};

#endif // MAINWINDOW_H
