#include "qgphotovideoinputdevicecontrol.h"

QGPhotoVideoInputDeviceControl::QGPhotoVideoInputDeviceControl(QGPhotoCaptureSession *captureSession, QObject *parent) :
    QVideoDeviceSelectorControl(parent),
    m_captureSession(captureSession)
{
}




void QGPhotoVideoInputDeviceControl::setSelectedDevice(int index)
{

}

