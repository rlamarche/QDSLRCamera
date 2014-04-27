#ifndef QGPHOTOCAPTURESESSION_H
#define QGPHOTOCAPTURESESSION_H

#include <QObject>
#include <QList>
#include <QStringList>

#include <gphoto2/gphoto2-context.h>
#include <gphoto2/gphoto2-camera.h>

class QGPhotoCaptureSession : public QObject
{
    Q_OBJECT
public:
    enum State { StoppedState, ReadyState, ErrorState, BusyState };

    explicit QGPhotoCaptureSession(QObject *parent = 0);
    ~QGPhotoCaptureSession();

    int deviceCount() const;

    QString deviceName(int index) const;
    QString deviceDescription(int index) const;

    int defaultDevice() const;
    int selectedDevice() const;

    void setSelectedDevice(int index);
    void openDevice();

    void captureImage(int reqId, const QString &fileName);

    QGPhotoCaptureSession::State state();
private:
    void updateDevices() const;

    void gpError(int gpStatus);

    GPContext *m_gpContext;
    Camera* m_gpCamera;

    mutable QList<QByteArray> m_cameraDevices;
    mutable QStringList m_cameraDescriptions;


    int m_selectedDevice;

    QGPhotoCaptureSession::State m_state;

signals:
    void stateChanged(QGPhotoCaptureSession::State state);
    void error(int error, const QString &errorString);

public slots:

};

#endif // QGPHOTOCAPTURESESSION_H
