#include "videostreamer.h"
#include <QDebug>

VideoStreamer::VideoStreamer(QObject *parent) : QObject(parent)
{
    mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    videoWidget = new QVideoWidget();
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mediaPlayer->setVideoOutput(videoWidget);

    connect(mediaPlayer, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {
        emit streamStateChanged(state == QMediaPlayer::PlayingState);
    });

    connect(mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
        [this](QMediaPlayer::Error error) {
            qDebug() << "Media player error:" << error;
            emit streamStateChanged(false);
        });
}

VideoStreamer::~VideoStreamer()
{
    stopStream();
    delete videoWidget;
}

QWidget* VideoStreamer::getVideoWidget()
{
    return videoWidget;
}

void VideoStreamer::startStream(const QString &url)
{
    mediaPlayer->setMedia(QUrl(url));
    mediaPlayer->play();
}

void VideoStreamer::stopStream()
{
    mediaPlayer->stop();
}
