#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>

class VideoStreamer : public QObject
{
    Q_OBJECT

public:
    explicit VideoStreamer(QObject *parent = nullptr);
    ~VideoStreamer();

    QWidget* getVideoWidget();
    void startStream(const QString &url);
    void stopStream();

signals:
    void streamStateChanged(bool isPlaying);

private:
    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
};

#endif // VIDEOSTREAMER_H
