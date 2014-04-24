#include "qgphotocaptureservice.h"

#include <iostream>

#include <QCameraControl>

QGPhotoCaptureService::QGPhotoCaptureService(const QString &service, QObject *parent) :
    QMediaService(parent)
{

}

QGPhotoCaptureService::~QGPhotoCaptureService()
{

}

QMediaControl *QGPhotoCaptureService::requestControl(const char *name)
{
    std::cout << "Request control : " << name << "\n";
    std::cout.flush();

    if (qstrcmp(name,QCameraControl_iid) == 0)
        return m_cameraControl;

    if (qstrcmp(name, QCameraImageCaptureControl_iid) == 0)
        return m_imageCaptureControl;

    return 0;
}

/*
Request control : org.qt-project.qt.metadatareadercontrol/5.0
Request control : org.qt-project.qt.mediaavailabilitycontrol/5.0
Request control : org.qt-project.qt.cameracontrol/5.0
Request control : org.qt-project.qt.cameralockscontrol/5.0
Request control : org.qt-project.qt.videodeviceselectorcontrol/5.0
Request control : org.qt-project.qt.cameraexposurecontrol/5.0
Request control : org.qt-project.qt.cameraflashcontrol/5.0
Request control : org.qt-project.qt.camerafocuscontrol/5.0
Request control : org.qt-project.qt.camerazoomcontrol/5.0
Request control : org.qt-project.qt.cameraimageprocessingcontrol/5.0
*/

void QGPhotoCaptureService::releaseControl(QMediaControl *control)
{

}
