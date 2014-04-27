#include "qgphotovideoinputdevicecontrol.h"

#include <QtCore/QDebug>

QGPhotoVideoInputDeviceControl::QGPhotoVideoInputDeviceControl(QGPhotoCaptureSession *captureSession) :
    QVideoDeviceSelectorControl(captureSession),
    m_captureSession(captureSession)
{
}

QGPhotoVideoInputDeviceControl::~QGPhotoVideoInputDeviceControl() {

}

int QGPhotoVideoInputDeviceControl::deviceCount() const {
    qDebug() << "QGPhotoVideoInputDeviceControl::deviceCount";

    return m_captureSession->deviceCount();
}

QString QGPhotoVideoInputDeviceControl::deviceName(int index) const {
    qDebug() << "QGPhotoVideoInputDeviceControl::deviceName";

    return m_captureSession->deviceName(index);
}

QString QGPhotoVideoInputDeviceControl::deviceDescription(int index) const {
    qDebug() << "QGPhotoVideoInputDeviceControl::deviceDescription";

    return m_captureSession->deviceDescription(index);
}

int QGPhotoVideoInputDeviceControl::defaultDevice() const {
    qDebug() << "QGPhotoVideoInputDeviceControl::defaultDevice";

    return m_captureSession->defaultDevice();
}

int QGPhotoVideoInputDeviceControl::selectedDevice() const {
    qDebug() << "QGPhotoVideoInputDeviceControl::selectedDevice";

    return m_captureSession->selectedDevice();
}

void QGPhotoVideoInputDeviceControl::setSelectedDevice(int index)
{
    qDebug() << "QGPhotoVideoInputDeviceControl::setSelectedDevice " << index;
    m_captureSession->setSelectedDevice(index);
}

