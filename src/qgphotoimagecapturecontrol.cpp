#include "qgphotoimagecapturecontrol.h"

QGPhotoImageCaptureControl::QGPhotoImageCaptureControl(QObject *parent) :
    QCameraImageCaptureControl(parent)
{
}


bool QGPhotoImageCaptureControl::isReadyForCapture() const {
    // TODO

    return true;
}

QCameraImageCapture::DriveMode QGPhotoImageCaptureControl::driveMode() const {
    // TODO

    return QCameraImageCapture::SingleImageCapture;
}

void QGPhotoImageCaptureControl::setDriveMode(QCameraImageCapture::DriveMode mode) {
    // TODO
}

int QGPhotoImageCaptureControl::capture(const QString &fileName) {
    // TODO

    return 0;
}

void QGPhotoImageCaptureControl::cancelCapture() {
    // TODO
}
