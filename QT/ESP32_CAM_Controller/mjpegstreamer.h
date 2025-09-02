#ifndef MJPEGSTREAMER_H
#define MJPEGSTREAMER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLabel>

class MjpegStreamer : public QObject
{
    Q_OBJECT
public:
    explicit MjpegStreamer(QLabel *displayLabel, QObject *parent = nullptr);
    ~MjpegStreamer();

    void startStream(const QString &url);
    void stopStream();

signals:
    void errorOccurred(const QString &message);

private slots:
    void readData();
    void handleError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager *networkManager;
    QLabel *displayLabel;
    QNetworkReply *currentReply;
    QByteArray buffer;

    void processBuffer();
};

#endif // MJPEGSTREAMER_H
