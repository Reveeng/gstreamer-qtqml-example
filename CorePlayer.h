#ifndef COREPLAYER_H
#define COREPLAYER_H

#include <QObject>
#include <QVideoSurfaceFormat>
#include <QAbstractVideoSurface>
#include <QTimer>

class reconnectTimer : public QObject
{

    Q_OBJECT
public:
    explicit reconnectTimer(int frameDelay, int lostConDelay, QObject* parent = nullptr):
        QObject(parent),
        m_frameTimer(new QTimer(this)),
        m_restoreConnectionTimer(new QTimer(this)),
        m_frameDelay(frameDelay),
        m_lostConDelay(lostConDelay)
    {
        m_restoreConnectionTimer->setSingleShot(true);
        connect(m_frameTimer, &QTimer::timeout, this, &reconnectTimer::reconnect);
        connect(m_restoreConnectionTimer, &QTimer::timeout, m_frameTimer, &QTimer::stop);
        connect(m_restoreConnectionTimer, &QTimer::timeout, this,&reconnectTimer::connectionLost);
    }
    ~reconnectTimer(){

    }

    void start(){
        m_restoreConnectionTimer->start(m_lostConDelay);
        m_frameTimer->start(m_frameDelay);
    }
    void stop(){
        m_restoreConnectionTimer->stop();
        m_frameTimer->stop();
    }

signals:
    void reconnect();
    void connectionLost();

private:
    QTimer* m_frameTimer;
    QTimer* m_restoreConnectionTimer;
    int m_frameDelay;
    int m_lostConDelay;
};


class BasePlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QAbstractVideoSurface * videoSurface READ videoSurface WRITE setVideoSurface)

public:

    enum stateE{
        Playing,
        Paused,
        Stoped,
        Error
    };
    Q_ENUM(stateE);



    explicit BasePlayer(QObject* parent = nullptr);
    ~BasePlayer();

    QAbstractVideoSurface * videoSurface() const;

private slots:

    void createFrameCircle();

protected slots:

    QVideoFrame* getPtrFromFrameCircle();

public slots:
    void setVideoSurface(QAbstractVideoSurface * surface);

    void updateFrame(QVideoFrame *frame);

    void setFormat(QVideoSurfaceFormat format);

    void setEState(stateE state);

    void closeSurface();

signals:

    void eStateChanged(stateE state);

    void errorRaised();

    void reconnect();

    void connectionLost();

private:

    QAbstractVideoSurface* m_surface;

    QVideoSurfaceFormat m_format;

    stateE m_eState;

    QVector<QVideoFrame*> m_frameCircle;

    uint8_t m_frameIndex;

    reconnectTimer* m_recTimer;

protected:

    int m_bpp;

};

#endif // COREPLAYER_H
