#include "CorePlayer.h"
#include <QDebug>

BasePlayer::BasePlayer(QObject* parent): QObject(parent),
    m_surface(nullptr),
    m_eState(BasePlayer::Stoped),
    m_frameIndex(0),
    m_recTimer(new reconnectTimer(10000,60000, this))
{
    connect(m_recTimer, &reconnectTimer::reconnect, this, [](){qDebug() << "connection lost, reconnect called";});
    connect(m_recTimer, &reconnectTimer::connectionLost, this, [](){qDebug() << "connection lost, no connection to device";});

    connect(m_recTimer, &reconnectTimer::reconnect, this, &BasePlayer::reconnect);
    connect(m_recTimer, &reconnectTimer::connectionLost, this, &BasePlayer::connectionLost);
}



BasePlayer::~BasePlayer(){
    closeSurface();
    while(m_frameCircle.count()){
        delete m_frameCircle.back();
        m_frameCircle.pop_back();
    }
}

QAbstractVideoSurface* BasePlayer::videoSurface() const{
    return m_surface;
}

void BasePlayer::setVideoSurface(QAbstractVideoSurface* surface){
    if (m_surface)
        closeSurface();
    m_surface = surface;
    m_surface->start(m_format);
}

void BasePlayer::closeSurface(){
    if (m_surface && m_surface->isActive()){
        m_surface->stop();
        m_eState = BasePlayer::Stoped;
    }
}

void BasePlayer::setFormat(QVideoSurfaceFormat format){
    m_format = format;
    createFrameCircle();
}

void BasePlayer::setEState(stateE state){
    if (m_eState == state)
        return;
    m_eState = state;
    if (m_eState == BasePlayer::Error){
        emit errorRaised();
        m_recTimer->stop();
    }
    if (m_eState == BasePlayer::Playing){
        m_recTimer->start();
    }
    emit eStateChanged(m_eState);
}

void BasePlayer::createFrameCircle(){
    QSize size = m_format.frameSize();
    QVideoFrame::PixelFormat format = m_format.pixelFormat();
    QVideoFrame* frame;
    for (int i = 0; i < 6; i++){
        frame = new QVideoFrame(size.width()*size.height()*m_bpp,size,size.width()*m_bpp,format);
        m_frameCircle.append(frame);
    }
}

void BasePlayer::updateFrame(QVideoFrame* frame){
    if (!m_surface->present(*frame)){
        m_eState = BasePlayer::Error;
    }
    else{
        m_eState = BasePlayer::Playing;
        m_recTimer->start();
    }
}

QVideoFrame* BasePlayer::getPtrFromFrameCircle(){
    QVideoFrame* frame = m_frameCircle[m_frameIndex];
    m_frameIndex++;
    if (m_frameIndex >= m_frameCircle.count())
        m_frameIndex = 0;
    return frame;
}
