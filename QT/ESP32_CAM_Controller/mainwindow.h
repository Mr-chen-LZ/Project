#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

class MjpegStreamer;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_forwardButton_pressed();
    void on_forwardButton_released();
    void on_backwardButton_pressed();
    void on_backwardButton_released();
    void on_leftButton_pressed();
    void on_leftButton_released();
    void on_rightButton_pressed();
    void on_rightButton_released();
    void on_lightOnButton_clicked();
    void on_lightOffButton_clicked();
    void handleStreamError(const QString &message);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    MjpegStreamer *streamer;
    QString esp32IP;

    void sendCommand(const QString &command);
    void setupControlButtons(bool enabled);
};
#endif // MAINWINDOW_H
