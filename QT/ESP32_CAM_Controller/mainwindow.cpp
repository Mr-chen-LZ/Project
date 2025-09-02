#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mjpegstreamer.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , streamer(nullptr)
{
    ui->setupUi(this);

    // 设置按钮样式
    QString buttonStyle = "QPushButton {"
                         "  background-color: #2f4468;"
                         "  color: white;"
                         "  border: none;"
                         "  padding: 10px 20px;"
                         "  font-size: 16px;"
                         "  border-radius: 5px;"
                         "}"
                         "QPushButton:pressed {"
                         "  background-color: #1a2d4d;"
                         "}"
                         "QPushButton:disabled {"
                         "  background-color: #cccccc;"
                         "}";

    ui->forwardButton->setStyleSheet(buttonStyle);
    ui->backwardButton->setStyleSheet(buttonStyle);
    ui->leftButton->setStyleSheet(buttonStyle);
    ui->rightButton->setStyleSheet(buttonStyle);
    ui->lightOnButton->setStyleSheet(buttonStyle);
    ui->lightOffButton->setStyleSheet(buttonStyle);
    ui->connectButton->setStyleSheet(buttonStyle);

    // 初始禁用控制按钮
    setupControlButtons(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete streamer;
}

void MainWindow::on_connectButton_clicked()
{
    esp32IP = ui->ipLineEdit->text().trimmed();
    if(esp32IP.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入ESP32的IP地址");
        return;
    }

    // 创建流处理器
    if(!streamer) {
        streamer = new MjpegStreamer(ui->videoLabel, this);
        connect(streamer, &MjpegStreamer::errorOccurred, this, &MainWindow::handleStreamError);
    }

    // 启动视频流
    QString streamUrl = "http://" + esp32IP + ":81/stream";
    streamer->startStream(streamUrl);

    // 测试控制连接
    sendCommand("stop"); // 发送一个无害的命令测试连接

    // 启用控制按钮
    setupControlButtons(true);
}

void MainWindow::handleStreamError(const QString &message)
{
    QMessageBox::warning(this, "cuowu", "wfjs:\n" + message);
    setupControlButtons(false);
}

void MainWindow::setupControlButtons(bool enabled)
{
    ui->forwardButton->setEnabled(enabled);
    ui->backwardButton->setEnabled(enabled);
    ui->leftButton->setEnabled(enabled);
    ui->rightButton->setEnabled(enabled);
    ui->lightOnButton->setEnabled(enabled);
    ui->lightOffButton->setEnabled(enabled);
}

void MainWindow::sendCommand(const QString &command)
{
    if(esp32IP.isEmpty()) return;

    QString url = "http://" + esp32IP + "/action?go=" + command;
    QNetworkReply *reply = networkManager->get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, [reply]() {
        reply->deleteLater();
    });
}

// 控制按钮槽函数
void MainWindow::on_forwardButton_pressed() { sendCommand("backward"); }
void MainWindow::on_forwardButton_released() { sendCommand("stop"); }
void MainWindow::on_backwardButton_pressed() { sendCommand("forward"); }
void MainWindow::on_backwardButton_released() { sendCommand("stop"); }
void MainWindow::on_leftButton_pressed() { sendCommand("right"); }
void MainWindow::on_leftButton_released() { sendCommand("stop"); }
void MainWindow::on_rightButton_pressed() { sendCommand("left"); }
void MainWindow::on_rightButton_released() { sendCommand("stop"); }
void MainWindow::on_lightOnButton_clicked() { sendCommand("ledon"); }
void MainWindow::on_lightOffButton_clicked() { sendCommand("ledoff"); }
