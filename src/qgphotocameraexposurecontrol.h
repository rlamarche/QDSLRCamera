#ifndef QGPHOTOCAMERAEXPOSURECONTROL_H
#define QGPHOTOCAMERAEXPOSURECONTROL_H

#include <QCameraExposureControl>

#include "qgphotocapturesession.h"

class QGPhotoCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    explicit QGPhotoCameraExposureControl(QGPhotoCaptureSession *captureSession);

    bool isParameterSupported(ExposureParameter parameter) const ;
    QVariantList supportedParameterRange(ExposureParameter parameter, bool *continuous) const;

    QVariant requestedValue(ExposureParameter parameter) const;
    QVariant actualValue(ExposureParameter parameter) const;
    bool setValue(ExposureParameter parameter, const QVariant& value);


private:
    QGPhotoCaptureSession *m_captureSession;
signals:

public slots:

};

#endif // QGPHOTOCAMERAEXPOSURECONTROL_H
