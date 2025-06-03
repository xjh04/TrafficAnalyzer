#include "RegisterWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("QWidget { background-color: #f0f0f0; }");
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    
    // 标题
    auto *titleLabel = new QLabel("用户注册");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; margin: 20px;");
    
    // 注册框
    auto *registerBox = new QGroupBox("注册新用户");
    registerBox->setFixedSize(400, 300);
    registerBox->setStyleSheet("QGroupBox { font-size: 14px; font-weight: bold; padding: 10px; }");
    
    auto *formLayout = new QFormLayout(registerBox);
    
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");
    
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");
    
    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText("请确认密码");
    confirmPasswordEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");
    
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("请输入邮箱");
    emailEdit->setStyleSheet("padding: 8px; border: 1px solid #ccc; border-radius: 4px;");
    
    formLayout->addRow("用户名:", usernameEdit);
    formLayout->addRow("密码:", passwordEdit);
    formLayout->addRow("确认密码:", confirmPasswordEdit);
    formLayout->addRow("邮箱:", emailEdit);
    
    // 按钮
    auto *buttonLayout = new QHBoxLayout();
    
    registerBtn = new QPushButton("注册");
    registerBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 8px 20px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #219a52; }");
    
    backBtn = new QPushButton("返回登录");
    backBtn->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; padding: 8px 20px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #7f8c8d; }");
    
    buttonLayout->addWidget(registerBtn);
    buttonLayout->addWidget(backBtn);
    
    formLayout->addRow(buttonLayout);
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(registerBox);
    
    // 连接信号槽
    connect(registerBtn, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
    connect(backBtn, &QPushButton::clicked, this, &RegisterWidget::onBackClicked);
}

void RegisterWidget::onRegisterClicked()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    QString email = emailEdit->text();
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "警告", "所有字段都不能为空！");
        return;
    }
    
    if (password != confirmPassword) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致！");
        return;
    }
    
    // 模拟注册成功
    QMessageBox::information(this, "成功", "注册成功！请返回登录。");
    
    // 清空表单
    usernameEdit->clear();
    passwordEdit->clear();
    confirmPasswordEdit->clear();
    emailEdit->clear();
}

void RegisterWidget::onBackClicked()
{
    emit backToLogin();
}
