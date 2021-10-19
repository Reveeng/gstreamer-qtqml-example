#include "GstBinding.h"
#include <QDebug>

static GstFlowReturn OnAppsinkNewFrame(GstElement *sink, GstVideoPlayer *videoplayer)
{
    Q_UNUSED(sink)
    return (GstFlowReturn)videoplayer->pullAppsinkFrame();
}

GstVideoPlayer::GstVideoPlayer(QObject * parent) : BasePlayer(parent),
    m_pipeline(nullptr),
    m_appsink(nullptr),
    m_sinkName("sink0")
{
    gst_init(NULL, NULL);

    connect(this, &GstVideoPlayer::newFrame, this, &GstVideoPlayer::updateFrame);
    connect(this, &GstVideoPlayer::sourceChanged, this, &GstVideoPlayer::startByBtn);
    connect(this, &GstVideoPlayer::reconnect, this, &GstVideoPlayer::tryToReconnect);
    connect(this, &GstVideoPlayer::startedByBtn, this, [=](){this->setEState(BasePlayer::Playing);});
}
GstVideoPlayer::~GstVideoPlayer(){
    stop();
}

void GstVideoPlayer::tryToReconnect(){
    stop();
    start();
}

QString GstVideoPlayer::source() const{
    return m_source;
}

void GstVideoPlayer::setSource(QString source){
     if(m_source == source)
         return;
     m_source = source;
     emit sourceChanged();
}

void GstVideoPlayer::startByBtn(){
    emit startedByBtn();
    start();
}

void GstVideoPlayer::start(){
    qDebug() << "start video player";

    m_pipeline = gst_parse_launch(qPrintable(m_source),NULL);
    if (!m_pipeline){
        qCritical() << "Failed to start pipeline";
        setEState(BasePlayer::Error);
        return;
    }

    //get appsink from gstreamer pipeline
    m_appsink = gst_bin_get_by_name(GST_BIN(m_pipeline), qPrintable(m_sinkName));
    if (!m_appsink){
        qCritical() << "Failed to get appsrc";
        gst_object_unref(m_pipeline);
        setEState(BasePlayer::Error);
        return;
    }

    //connecting gstreamer signal to callback functions
    //new-sample emiting when pipeline get new frame from source
    g_signal_connect (m_appsink, "new-sample", G_CALLBACK (OnAppsinkNewFrame), this);


    //set gstreamer pipeline to playing state
    GstStateChangeReturn res = gst_element_set_state(GST_ELEMENT(m_pipeline), GST_STATE_PLAYING);
    if (res == GST_STATE_CHANGE_FAILURE)
    {
        qCritical() << "Unable to set the pipeline to the playing state";
        gst_object_unref(m_appsink);
        gst_object_unref(m_pipeline);
        setEState(BasePlayer::Error);
        return;
    }
    emit started();

}

void GstVideoPlayer::stop(){

    //set gstreamer pipelin to null state(stop state)
    if (m_pipeline)
        gst_element_set_state(GST_ELEMENT(m_pipeline), GST_STATE_NULL);

    //unref all g_object used is programm
    if (m_appsink) {
        gst_object_unref(m_appsink);
        m_appsink = nullptr;
    }
    if(m_pipeline) {
        gst_object_unref(m_pipeline);
        m_pipeline = nullptr;
    }
    qDebug() << "video player stopped";
    setEState(BasePlayer::Stoped);
    emit stopped();
}

int GstVideoPlayer::pullAppsinkFrame(){
    GstSample *sample;
    GstBuffer *buf;
    GstMapInfo Ginfo;

    g_signal_emit_by_name (m_appsink, "pull-sample", &sample);
    if (! sample) {
        qCritical() << "Cant get frame from sample";
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    QVideoFrame* frame = getPtrFromFrameCircle();
    if (!frame){
        qCritical() << "no buffers in QVideoFrame pool";
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    buf = gst_sample_get_buffer(sample);
    if (!buf){
        qCritical() << "Unable to get buffer";
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    if (!gst_buffer_map(GST_BUFFER(buf),&Ginfo, GST_MAP_READ)){
        qCritical() << "unable to map GstBuffer";
        gst_sample_unref(sample);
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    if (frame->isMapped())
        frame->unmap();


    if (!frame->map(QAbstractVideoBuffer::ReadWrite)){
        qCritical() << "Unable to map QVideoFrame";
        gst_buffer_unmap(buf,&Ginfo);
        gst_sample_unref (sample);
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    //check if source format and frame format match
//    //qDebug() << info.size;
    if (frame->mappedBytes() != int(Ginfo.size)){
        //qDebug() << frame->mappedBytes() << info.size;
        qCritical() << "source format and QVideoFrame format dont match";
        frame->unmap();
        gst_buffer_unmap(buf,&Ginfo);
        gst_sample_unref (sample);
        setEState(BasePlayer::Error);
        return GST_FLOW_ERROR;
    }

    //copy data from gstreamer buffer to buffer from QVideoFrame
    memcpy(frame->bits(), Ginfo.data, Ginfo.size);

    //unmapping and release buffers
    frame->unmap();

    gst_buffer_unmap(buf,&Ginfo);
    gst_sample_unref (sample);

    emit newFrame(frame);
    return GST_FLOW_OK;
}
