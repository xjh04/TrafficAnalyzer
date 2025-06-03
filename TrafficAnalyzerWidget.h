#ifndef TRAFFICANALYZERWIDGET_H
#define TRAFFICANALYZERWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QProgressBar>

class TrafficAnalyzerWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit TrafficAnalyzerWidget(QWidget *parent = nullptr);

    private slots:
        void onStartAnalysis();
    void onStopAnalysis() const;
    void onClearResults() const;
    void onExportResults();

private:
    void setupUI();
    void addSampleData() const;

    QLineEdit *sourceEdit{};
    QComboBox *protocolCombo{};
    QPushButton *startBtn{};
    QPushButton *stopBtn{};
    QPushButton *clearBtn{};
    QPushButton *exportBtn{};
    QTableWidget *resultTable{};
    QTextEdit *logEdit{};
    QProgressBar *progressBar{};
    QLabel *statusLabel{};
    QLabel *statsLabel{};
};

#endif // TRAFFICANALYZERWIDGET_H