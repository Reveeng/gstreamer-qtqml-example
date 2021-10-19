#ifndef RAWPLAYER_H
#define RAWPLAYER_H

#include <QObject>
#include <QtQml/qqml.h>
#include "GstBinding.h"

class RawPlayer : public GstVideoPlayer
{
    Q_OBJECT

public:
    explicit RawPlayer(QObject * parent = nullptr) : GstVideoPlayer(parent)
    {
        m_bpp = 2;
        setFormat(QVideoSurfaceFormat(QSize(640,480), QVideoFrame::Format_UYVY));
        m_source = "rtspsrc protocols=tcp location=% latency=0 ! rtpvrawdepay ! queue ! appsink max-buffers=3 drop=true emit-signals=true name=sink0";
    };
};

#endif // RAWPLAYER_H
