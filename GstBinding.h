#ifndef GSTBINDING_H
#define GSTBINDING_H

#include <gst/gst.h>
#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QThread>
#include "CorePlayer.h"

class GstVideoPlayer : public BasePlayer
{
    Q_OBJECT

    Q_PROPERTY(QString uri READ uri WRITE setUri)
public:
    explicit GstVideoPlayer( QObject * parent = 0);
    ~GstVideoPlayer();

    int pullAppsinkFrame();

public slots:

    void startByBtn();

    void start();

    void stop();

    QString uri() const;

    void setUri(QString uri);

protected:

protected slots:

signals:
    void started();
    void stopped();
    void newFrame(QVideoFrame *frame);
    void startedByBtn();
    void uriChanged();

private slots:
    void tryToReconnect();

private:
    GstElement * m_pipeline;
    GstElement * m_appsink;
    QString m_launchstring;
    QString m_sinkName;
    ulong m_lastTimestamp;

protected:
    QString m_source;
    QString m_uri;
};


#endif // GSTBINDING_H
