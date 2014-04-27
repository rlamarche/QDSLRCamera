#ifndef QGPHOTOCAMERACONTROL_H
#define QGPHOTOCAMERACONTROL_H

#include <QObject>
#include <QCameraControl>

class QGPhotoCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    explicit QGPhotoCameraControl(QObject *parent = 0);

    QCamera::State state() const;
    void setState(QCamera::State state);

    QCamera::Status status() const;

    QCamera::CaptureModes captureMode() const;
    void setCaptureMode(QCamera::CaptureModes);
    bool isCaptureModeSupported(QCamera::CaptureModes mode) const;

    bool canChangeProperty(PropertyChangeType changeType, QCamera::Status status) const;


signals:

public slots:

};

#endif // QGPHOTOCAMERACONTROL_H
