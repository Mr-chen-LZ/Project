#include "mjpegstreamer.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImage>
#include <QPixmap>
#include <QDebug>

MjpegStreamer::MjpegStreamer(QLabel *displayLabel, QObject *parent)
    : QObject(parent), displayLabel(displayLabel), currentReply(nullptr)
{
    networkManager = new QNetworkAccessManager(this);
}

MjpegStreamer::~MjpegStreamer()
{
    stopStream();
    delete networkManager;
}

void MjpegStreamer::startStream(const QString &url)
{
    stopStream(); // 停止现有的流

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "multipart/x-mixed-replace");

    currentReply = networkManager->get(request);
    connect(currentReply, &QNetworkReply::readyRead, this, &MjpegStreamer::readData);
    connect(currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
           this, &MjpegStreamer::handleError);
    connect(currentReply, &QNetworkReply::finished, this, [this]() {
        if(currentReply->error() != QNetworkReply::NoError) {
            emit errorOccurred(currentReply->errorString());
        }
    });

    buffer.clear();
}

void MjpegStreamer::stopStream()
{
    if(currentReply) {
        currentReply->disconnect();
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = nullptr;
    }
    buffer.clear();
}

void MjpegStreamer::readData()
{
    buffer.append(currentReply->readAll());
    processBuffer();
}

void MjpegStreamer::handleError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    emit errorOccurred(currentReply->errorString());
}

void MjpegStreamer::processBuffer()
{
    // MJPEG流的边界是--123456789000000000000987654321
    const QByteArray boundary = "--123456789000000000000987654321";

    while(true) {
        int boundaryPos = buffer.indexOf(boundary);
        if(boundaryPos < 0) break;

        // 查找JPEG图像的开始和结束标记
        int jpegStart = buffer.indexOf("\xff\xd8", boundaryPos);
        if(jpegStart < 0) {
            buffer.remove(0, boundaryPos);
            break;
        }

        int jpegEnd = buffer.indexOf("\xff\xd9", jpegStart + 2);
        if(jpegEnd < 0) break; // 等待更多数据

        // 提取JPEG数据
        QByteArray jpegData = buffer.mid(jpegStart, jpegEnd - jpegStart + 2);

        // 创建图像并显示
        QImage image;
        if(image.loadFromData(jpegData, "JPEG") && !image.isNull()) {
            QPixmap pixmap = QPixmap::fromImage(image);
            if(displayLabel) {
                displayLabel->setPixmap(pixmap.scaled(
                    displayLabel->width(), displayLabel->height(),
                    Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }

        // 移除已处理的数据
        buffer.remove(0, jpegEnd + 2);
    }

    // 防止缓冲区过大
    if(buffer.size() > 1024 * 1024) {
        buffer.clear();
    }
}
