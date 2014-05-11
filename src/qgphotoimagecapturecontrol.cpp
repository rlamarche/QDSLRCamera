#include "qgphotoimagecapturecontrol.h"

#include <QtCore/QDebug>

QGPhotoImageCaptureControl::QGPhotoImageCaptureControl(QGPhotoCaptureSession *captureSession) :
    QCameraImageCaptureControl(captureSession) ,
    m_captureSession(captureSession),
    m_lastId(0)
{
    connect(m_captureSession, SIGNAL(imageAvailable(int,QVideoFrame)), this, SIGNAL(imageAvailable(int,QVideoFrame)));
    connect(m_captureSession, SIGNAL(imageCaptured(int,QImage)), this, SIGNAL(imageCaptured(int,QImage)));
    connect(m_captureSession, SIGNAL(imageExposed(int)), this, SIGNAL(imageExposed(int)));
    connect(m_captureSession, SIGNAL(imageMetadataAvailable(int,QString,QVariant)), this, SIGNAL(imageMetadataAvailable(int,QString,QVariant)));
    connect(m_captureSession, SIGNAL(imageSaved(int,QString)), this, SIGNAL(imageSaved(int,QString)));
    connect(m_captureSession, SIGNAL(captureError(int,int,QString)), this, SIGNAL(error(int,int,QString)));
}


bool QGPhotoImageCaptureControl::isReadyForCapture() const {
    QGPhotoCaptureSession::State state = m_captureSession->state();
    if (state == QGPhotoCaptureSession::ReadyState) {
        return true;
    } else {
        return false;
    }
}

QCameraImageCapture::DriveMode QGPhotoImageCaptureControl::driveMode() const {
    // TODO define more drive mode

    return QCameraImageCapture::SingleImageCapture;
}

void QGPhotoImageCaptureControl::setDriveMode(QCameraImageCapture::DriveMode mode) {
    // TODO implement drive mode on session

    qDebug() << "QGPhotoImageCaptureControl::setDriveMode";
}

int QGPhotoImageCaptureControl::capture(const QString &fileName) {
    qDebug() << "QGPhotoImageCaptureControl::capture " << fileName;

    m_lastId ++;
    m_captureSession->captureImage(m_lastId, fileName);

    return m_lastId;
}

void QGPhotoImageCaptureControl::cancelCapture() {
    // TODO

    qDebug() << "QGPhotoImageCaptureControl::cancelCapture";
}
