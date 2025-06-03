#include "MainWindow.h"
#include "LoginWidget.h"
#include "RegisterWidget.h"
#include "TrafficAnalyzerWidget.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("网络流量分析系统");
    setFixedSize(1000, 700);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // 创建各个界面
    loginWidget = new LoginWidget(this);
    registerWidget = new RegisterWidget(this);
    trafficWidget = new TrafficAnalyzerWidget(this);

    // 添加到堆栈窗口
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);
    stackedWidget->addWidget(trafficWidget);

    // 连接信号槽
    connect(loginWidget, &LoginWidget::registerRequested, this, &MainWindow::showRegister);
    connect(loginWidget, &LoginWidget::loginSuccess, this, &MainWindow::showTrafficAnalyzer);
    connect(registerWidget, &RegisterWidget::backToLogin, this, &MainWindow::showLogin);

    // 默认显示登录界面
    stackedWidget->setCurrentWidget(loginWidget);
}

MainWindow::~MainWindow()
= default;

void MainWindow::showRegister() const {
    stackedWidget->setCurrentWidget(registerWidget);
}

void MainWindow::showLogin() const {
    stackedWidget->setCurrentWidget(loginWidget);
}

void MainWindow::showTrafficAnalyzer() const {
    stackedWidget->setCurrentWidget(trafficWidget);
}
