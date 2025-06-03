#include "TrafficAnalyzerWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

TrafficAnalyzerWidget::TrafficAnalyzerWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    addSampleData();
}

void TrafficAnalyzerWidget::setupUI()
{
    setStyleSheet("QWidget { background-color: #f5f5f5; }");
    
    auto *mainLayout = new QVBoxLayout(this);
    
    // 标题
    auto *titleLabel = new QLabel("网络流量分析系统");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50; margin: 10px;");
    
    // 控制面板
    auto *controlGroup = new QGroupBox("控制面板");
    controlGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    auto *controlLayout = new QGridLayout(controlGroup);
    
    controlLayout->addWidget(new QLabel("数据源:"), 0, 0);
    sourceEdit = new QLineEdit();
    sourceEdit->setPlaceholderText("输入IP地址、文件路径或网络接口");
    sourceEdit->setStyleSheet("padding: 5px; border: 1px solid #ccc; border-radius: 3px;");
    controlLayout->addWidget(sourceEdit, 0, 1, 1, 2);
    
    controlLayout->addWidget(new QLabel("协议过滤:"), 1, 0);
    protocolCombo = new QComboBox();
    protocolCombo->addItems({"全部", "TCP", "UDP", "HTTP", "HTTPS", "FTP", "SSH", "DNS"});
    protocolCombo->setStyleSheet("padding: 5px; border: 1px solid #ccc; border-radius: 3px;");
    controlLayout->addWidget(protocolCombo, 1, 1);
    
    // 控制按钮
    auto *buttonLayout = new QHBoxLayout();
    
    startBtn = new QPushButton("开始分析");
    startBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; padding: 8px 15px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #219a52; }");
    
    stopBtn = new QPushButton("停止分析");
    stopBtn->setEnabled(false);
    stopBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; padding: 8px 15px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #c0392b; } QPushButton:disabled { background-color: #bdc3c7; }");
    
    clearBtn = new QPushButton("清空结果");
    clearBtn->setStyleSheet("QPushButton { background-color: #f39c12; color: white; padding: 8px 15px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #e67e22; }");
    
    exportBtn = new QPushButton("导出结果");
    exportBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 8px 15px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #2980b9; }");
    
    buttonLayout->addWidget(startBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(clearBtn);
    buttonLayout->addWidget(exportBtn);
    buttonLayout->addStretch();
    
    controlLayout->addLayout(buttonLayout, 2, 0, 1, 3);
    
    // 状态信息
    auto *statusLayout = new QHBoxLayout();
    statusLabel = new QLabel("状态: 就绪");
    statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    
    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #ccc; border-radius: 3px; } QProgressBar::chunk { background-color: #3498db; }");
    
    statsLabel = new QLabel("总计: 0 个包 | TCP: 0 | UDP: 0 | HTTP: 0");
    statsLabel->setStyleSheet("color: #7f8c8d;");
    
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(progressBar);
    statusLayout->addStretch();
    statusLayout->addWidget(statsLabel);
    
    // 结果表格
    auto *resultGroup = new QGroupBox("分析结果");
    resultGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    auto *resultLayout = new QVBoxLayout(resultGroup);
    
    resultTable = new QTableWidget();
    resultTable->setColumnCount(7);
    QStringList headers = {"时间", "源IP", "源端口", "目标IP", "目标端口", "协议", "流量类型"};
    resultTable->setHorizontalHeaderLabels(headers);
    resultTable->horizontalHeader()->setStretchLastSection(true);
    resultTable->setAlternatingRowColors(true);
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable->setStyleSheet("QTableWidget { gridline-color: #d0d0d0; } QHeaderView::section { background-color: #ecf0f1; font-weight: bold; }");
    
    resultLayout->addWidget(resultTable);
    
    // 日志区域
    auto *logGroup = new QGroupBox("系统日志");
    logGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    logGroup->setMaximumHeight(150);
    auto *logLayout = new QVBoxLayout(logGroup);
    
    logEdit = new QTextEdit();
    logEdit->setMaximumHeight(120);
    logEdit->setStyleSheet("QTextEdit { background-color: #2c3e50; color: #ecf0f1; font-family: 'Courier New', monospace; }");
    logEdit->append("系统启动完成，等待开始分析...");
    
    logLayout->addWidget(logEdit);
    
    // 添加到主布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(controlGroup);
    mainLayout->addLayout(statusLayout);
    mainLayout->addWidget(resultGroup, 1);
    mainLayout->addWidget(logGroup);
    
    // 连接信号槽
    connect(startBtn, &QPushButton::clicked, this, &TrafficAnalyzerWidget::onStartAnalysis);
    connect(stopBtn, &QPushButton::clicked, this, &TrafficAnalyzerWidget::onStopAnalysis);
    connect(clearBtn, &QPushButton::clicked, this, &TrafficAnalyzerWidget::onClearResults);
    connect(exportBtn, &QPushButton::clicked, this, &TrafficAnalyzerWidget::onExportResults);
}

void TrafficAnalyzerWidget::addSampleData() const {
    // 添加一些示例数据
    resultTable->setRowCount(5);
    
    QString sampleData[5][7] = {
        {"2024-01-15 10:30:45", "192.168.1.100", "8080", "10.0.0.1", "80", "HTTP", "Web浏览"},
        {"2024-01-15 10:30:46", "192.168.1.101", "443", "74.125.224.72", "443", "HTTPS", "加密Web"},
        {"2024-01-15 10:30:47", "192.168.1.102", "22", "10.0.0.2", "22", "SSH", "远程登录"},
        {"2024-01-15 10:30:48", "192.168.1.103", "53", "8.8.8.8", "53", "DNS", "域名解析"},
        {"2024-01-15 10:30:49", "192.168.1.104", "21", "192.168.1.200", "21", "FTP", "文件传输"}
    };
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 7; j++) {
            resultTable->setItem(i, j, new QTableWidgetItem(sampleData[i][j]));
        }
    }
    
    statsLabel->setText("总计: 5 个包 | TCP: 3 | UDP: 1 | HTTP: 1");
}

void TrafficAnalyzerWidget::onStartAnalysis()
{
    if (sourceEdit->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入数据源！");
        return;
    }
    
    startBtn->setEnabled(false);
    stopBtn->setEnabled(true);
    statusLabel->setText("状态: 正在分析...");
    statusLabel->setStyleSheet("color: #f39c12; font-weight: bold;");
    progressBar->setVisible(true);

    const QString msg = QString("开始分析数据源: %1, 协议过滤: %2")
                  .arg(sourceEdit->text())
                  .arg(protocolCombo->currentText());
    logEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - " + msg);
}

void TrafficAnalyzerWidget::onStopAnalysis() const {
    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);
    statusLabel->setText("状态: 已停止");
    statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    progressBar->setVisible(false);
    
    logEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - 分析已停止");
}

void TrafficAnalyzerWidget::onClearResults() const {
    resultTable->setRowCount(0);
    statsLabel->setText("总计: 0 个包 | TCP: 0 | UDP: 0 | HTTP: 0");
    logEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - 结果已清空");
}

void TrafficAnalyzerWidget::onExportResults()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出结果", "traffic_analysis_result.csv", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        QMessageBox::information(this, "成功", "结果已导出到: " + fileName);
        logEdit->append(QDateTime::currentDateTime().toString("hh:mm:ss") + " - 结果已导出: " + fileName);
    }
}
