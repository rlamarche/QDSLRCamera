#include "qgphotoimagecapturecontrol.h"

#include <QtCore/QDebug>

QGPhotoImageCaptureControl::QGPhotoImageCaptureControl(QGPhotoCaptureSession *captureSession) :
    QCameraImageCaptureControl(captureSession) ,
    m_captureSession(captureSession),
    m_lastId(0)
{
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
    qDebug() << "QGPhotoImageCaptureControl::capture " << filename;

    m_lastId ++;
    m_captureSession->captureImage(m_lastId, fileName);

    return m_lastId;
}

void QGPhotoImageCaptureControl::cancelCapture() {
    // TODO

    qDebug() << "QGPhotoImageCaptureControl::cancelCapture";
}
