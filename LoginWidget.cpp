#include "LoginWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGroupBox>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("QWidget { background-color: #f0f0f0; }");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 标题
    auto *titleLabel = new QLabel("网络流量分析系统");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin: 20px;");

    // 登录框
    auto *loginBox = new QGroupBox("用户登录");
    loginBox->setFixedSize(350, 200);
    loginBox->setStyleSheet("QGroupBox { font-size: 14px; font-weight: bold; padding: 10px; }");

    auto *formLayout = new QFormLayout(loginBox);

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");

    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");

    formLayout->addRow("用户名:", usernameEdit);
    formLayout->addRow("密码:", passwordEdit);

    // 按钮
    auto *buttonLayout = new QHBoxLayout();

    loginBtn = new QPushButton("登录");
    loginBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 8px 20px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #2980b9; }");

    registerBtn = new QPushButton("注册");
    registerBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; padding: 8px 20px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #7f8c8d; }");

    buttonLayout->addWidget(loginBtn);
    buttonLayout->addWidget(registerBtn);

    formLayout->addRow(buttonLayout);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(loginBox);

    // 连接信号槽
    connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &LoginWidget::onRegisterClicked);
}

void LoginWidget::onLoginClicked()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空！");
        return;
    }

    // 模拟登录验证
    if (username == "admin" && password == "123456") {
        QMessageBox::information(this, "成功", "登录成功！");
        emit loginSuccess();
    } else {
        QMessageBox::warning(this, "错误", "用户名或密码错误！");
    }
}

void LoginWidget::onRegisterClicked()
{
    emit registerRequested();
}
