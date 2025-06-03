#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class LoginWidget;
class RegisterWidget;
class TrafficAnalyzerWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    private slots:
        void showRegister() const;
    void showLogin() const;
    void showTrafficAnalyzer() const;

private:
    QStackedWidget *stackedWidget;
    LoginWidget *loginWidget;
    RegisterWidget *registerWidget;
    TrafficAnalyzerWidget *trafficWidget;
};

#endif // MAINWINDOW_H