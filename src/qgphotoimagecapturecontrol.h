#ifndef QGPHOTOIMAGECAPTURECONTROL_H
#define QGPHOTOIMAGECAPTURECONTROL_H

#include <QObject>
#include <QCameraImageCaptureControl>


class QGPhotoImageCaptureControl : public QCameraImageCaptureControl
{
    Q_OBJECT
public:
    explicit QGPhotoImageCaptureControl(QObject *parent = 0);

    bool isReadyForCapture() const;

    QCameraImageCapture::DriveMode driveMode() const;
    void setDriveMode(QCameraImageCapture::DriveMode mode);

    int capture(const QString &fileName);
    void cancelCapture();

signals:

public slots:

};

#endif // QGPHOTOIMAGECAPTURECONTROL_H
