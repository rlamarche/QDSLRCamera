#ifndef QGPHOTOCAMERACONTROL_H
#define QGPHOTOCAMERACONTROL_H

#include <QObject>
#include <QCameraControl>

#include "qgphotocapturesession.h"

class QGPhotoCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    explicit QGPhotoCameraControl(QGPhotoCaptureSession *captureSession);

    QCamera::State state() const;
    void setState(QCamera::State state);

    QCamera::Status status() const;

    QCamera::CaptureModes captureMode() const;
    void setCaptureMode(QCamera::CaptureModes);
    bool isCaptureModeSupported(QCamera::CaptureModes mode) const;

    bool canChangeProperty(PropertyChangeType changeType, QCamera::Status status) const;

private :
    QGPhotoCaptureSession *m_captureSession;

    QCamera::CaptureModes m_captureMode;
    QCamera::State m_state;
    QCamera::Status m_status;
signals:

public slots:

private slots:
    void updateStatus();
};

#endif // QGPHOTOCAMERACONTROL_H
