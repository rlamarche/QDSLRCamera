#ifndef QGPHOTOVIDEOINPUTDEVICECONTROL_H
#define QGPHOTOVIDEOINPUTDEVICECONTROL_H

#include <QObject>
#include <QVideoDeviceSelectorControl>

#include "qgphotocapturesession.h"

class QGPhotoVideoInputDeviceControl : public QVideoDeviceSelectorControl
{
    Q_OBJECT
public:
    explicit QGPhotoVideoInputDeviceControl(QGPhotoCaptureSession *captureSession);

    ~QGPhotoVideoInputDeviceControl();

    int deviceCount() const;

    QString deviceName(int index) const;
    QString deviceDescription(int index) const;

    int defaultDevice() const;
    int selectedDevice() const;

private:
    QGPhotoCaptureSession *m_captureSession;

signals:

public slots:
    void setSelectedDevice(int index);
};

#endif // QGPHOTOVIDEOINPUTDEVICECONTROL_H
