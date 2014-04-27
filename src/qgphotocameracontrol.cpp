#include "qgphotocameracontrol.h"

QGPhotoCameraControl::QGPhotoCameraControl(QObject *parent) :
    QCameraControl(parent)
{
}

QCamera::State QGPhotoCameraControl::state() const {

    // TODO

    return QCamera::LoadedState;
}

void QGPhotoCameraControl::setState(QCamera::State state) {
    // TODO
}

QCamera::Status QGPhotoCameraControl::status() const {
    // TODO

    return QCamera::LoadedStatus;
}

QCamera::CaptureModes QGPhotoCameraControl::captureMode() const {
    // TODO
    return QCamera::CaptureStillImage;
}

void QGPhotoCameraControl::setCaptureMode(QCamera::CaptureModes) {
    // TODO
}

bool QGPhotoCameraControl::isCaptureModeSupported(QCamera::CaptureModes mode) const {
    // TODO

    return true;
}

bool QGPhotoCameraControl::canChangeProperty(PropertyChangeType changeType, QCamera::Status status) const {
    // TODO

    return true;
}
