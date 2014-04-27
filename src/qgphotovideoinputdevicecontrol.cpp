#include "qgphotovideoinputdevicecontrol.h"

QGPhotoVideoInputDeviceControl::QGPhotoVideoInputDeviceControl(QGPhotoCaptureSession *captureSession, QObject *parent) :
    QVideoDeviceSelectorControl(parent),
    m_captureSession(captureSession)
{
}

QGPhotoVideoInputDeviceControl::~QGPhotoVideoInputDeviceControl() {

}

int QGPhotoVideoInputDeviceControl::deviceCount() const {
    // TODO

    return 0;
}

QString QGPhotoVideoInputDeviceControl::deviceName(int index) const {
    // TODO

    return QString("FakeDevice");
}

QString QGPhotoVideoInputDeviceControl::deviceDescription(int index) const {
    // TODO

    return QString("Fake description");
}

int QGPhotoVideoInputDeviceControl::defaultDevice() const {
    // TODO

    return 0;
}

int QGPhotoVideoInputDeviceControl::selectedDevice() const {
    // TODO

    return 0;
}

void QGPhotoVideoInputDeviceControl::setSelectedDevice(int index)
{
    // TODO
}

