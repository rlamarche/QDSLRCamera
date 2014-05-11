#ifndef QGPHOTOVIDEORENDERERCONTROL_H
#define QGPHOTOVIDEORENDERERCONTROL_H

#include <QVideoRendererControl>
#include "qgphotocapturesession.h"

class QGPhotoVideoRendererControl : public QVideoRendererControl
{
    Q_OBJECT
public:
    explicit QGPhotoVideoRendererControl(QGPhotoCaptureSession *captureSession);

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);
private:
    QGPhotoCaptureSession *m_captureSession;
    QAbstractVideoSurface *m_videoSurface;
signals:

public slots:

};

#endif // QGPHOTOVIDEORENDERERCONTROL_H
