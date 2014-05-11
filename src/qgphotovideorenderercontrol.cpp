#include "qgphotovideorenderercontrol.h"

#include <QtCore/QDebug>

QGPhotoVideoRendererControl::QGPhotoVideoRendererControl(QGPhotoCaptureSession *captureSession) :
    QVideoRendererControl(captureSession) ,
    m_captureSession(captureSession)
{
}

QAbstractVideoSurface* QGPhotoVideoRendererControl::surface() const {
    return m_videoSurface;
}

void QGPhotoVideoRendererControl::setSurface(QAbstractVideoSurface *surface) {
    m_videoSurface = surface;

    m_captureSession->setSurface(surface);
}
