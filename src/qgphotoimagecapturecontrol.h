#ifndef QGPHOTOIMAGECAPTURECONTROL_H
#define QGPHOTOIMAGECAPTURECONTROL_H

#include <QObject>
#include <QCameraImageCaptureControl>
#include "qgphotocapturesession.h"

class QGPhotoImageCaptureControl : public QCameraImageCaptureControl
{
    Q_OBJECT
public:
    explicit QGPhotoImageCaptureControl(QGPhotoCaptureSession *captureSession);

    bool isReadyForCapture() const;

    QCameraImageCapture::DriveMode driveMode() const;
    void setDriveMode(QCameraImageCapture::DriveMode mode);

    int capture(const QString &fileName);
    void cancelCapture();

private:
    QGPhotoCaptureSession *m_captureSession;

    int m_lastId;
signals:

public slots:

};

#endif // QGPHOTOIMAGECAPTURECONTROL_H
