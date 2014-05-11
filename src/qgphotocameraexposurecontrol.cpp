#include "qgphotocameraexposurecontrol.h"

#include <QtCore/QDebug>

QGPhotoCameraExposureControl::QGPhotoCameraExposureControl(QGPhotoCaptureSession *captureSession) :
    QCameraExposureControl(captureSession),
    m_captureSession(captureSession)
{
}

bool QGPhotoCameraExposureControl::isParameterSupported(ExposureParameter parameter) const
{
    bool isSupported = false;
    switch (parameter) {
    case ISO:
        qDebug("isParameterSupported ISO");
        isSupported = true;
        break;
    case Aperture:
        qDebug("isParameterSupported Aperture");
        isSupported = true;
        break;
   case ShutterSpeed:
        qDebug("isParameterSupported ShutterSpeed");
        isSupported = true;
        break;
    case ExposureCompensation:
        qDebug("isParameterSupported ExposureCompensation");
        isSupported = true;
        break;
    case FlashPower:
        qDebug("isParameterSupported FlashPower");
        isSupported = true;
        break;
    case FlashCompensation:
        qDebug("isParameterSupported FlashCompensation");
        isSupported = true;
        break;
    case TorchPower:
        qDebug("isParameterSupported TorchPower");
        isSupported = false;
        break;
    case SpotMeteringPoint:
        qDebug("isParameterSupported SpotMeteringPoint");
        isSupported = true;
        break;
    case ExposureMode:
        qDebug("isParameterSupported ExposureMode");
        isSupported = true;
        break;
    case MeteringMode:
        qDebug("isParameterSupported MeteringMode");
        isSupported = true;
        break;
    case ExtendedExposureParameter:
        qDebug("isParameterSupported ExtendedExposureParameter");
        isSupported = false;
        break;
    default: break;
    }

    return isSupported;
}

QVariantList QGPhotoCameraExposureControl::supportedParameterRange(ExposureParameter parameter, bool *continuous) const
{
   return m_captureSession->supportedExposureParameterRange(parameter, continuous);
}

QVariant QGPhotoCameraExposureControl::requestedValue(ExposureParameter parameter) const
{
    return QVariant::Invalid;
}

QVariant QGPhotoCameraExposureControl::actualValue(ExposureParameter parameter) const
{
    return m_captureSession->actualExposureValue(parameter);
}

bool QGPhotoCameraExposureControl::setValue(ExposureParameter parameter, const QVariant& value)
{
    return m_captureSession->setExposureValue(parameter, value);
}
