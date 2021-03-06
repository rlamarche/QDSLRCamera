#ifndef QGPHOTOCAPTURESESSION_H
#define QGPHOTOCAPTURESESSION_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QImage>
#include <QRegExp>

#include <QAbstractVideoSurface>
#include <QCameraExposureControl>

#include <gphoto2/gphoto2-context.h>
#include <gphoto2/gphoto2-camera.h>


class QGPhotoViewFinderThread;

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
    void closeDevice();

    void startViewFinder();
    void stopViewFinder();
    void pauseViewFinder();
    void continueViewFinder();

    void captureImage(int reqId, const QString &fileName);
    QImage capturePreview();

    QVariant actualExposureValue(QCameraExposureControl::ExposureParameter parameter);
    bool setExposureValue(QCameraExposureControl::ExposureParameter parameter, const QVariant& value);
    QVariantList supportedExposureParameterRange(QCameraExposureControl::ExposureParameter parameter, bool *continuous);

    QGPhotoCaptureSession::State state();

    void setSurface(QAbstractVideoSurface *surface);
private:
    void updateDevices() const;

    void gpError(int gpStatus, QString message);

    void setWidgetValue(QString name, QVariant value);
    QVariantList getWidgetRange(QString name);
    void getWidgetValue(QString name, void *value);

    QString exposureParameterToWidgetName(QCameraExposureControl::ExposureParameter parameter) const;
    QVariant exposureLabelToValue(QCameraExposureControl::ExposureParameter parameter, QVariant value) const;

    GPContext *m_gpContext;
    Camera *m_gpCamera;
    CameraWidget *m_gpWindow;
    bool m_viewFinderRunning;

    mutable QList<QByteArray> m_cameraDevices;
    mutable QStringList m_cameraDescriptions;


    int m_selectedDevice;
    bool m_deviceChanged;

    QGPhotoCaptureSession::State m_state;

    QAbstractVideoSurface *m_videoSurface;

    QGPhotoViewFinderThread *m_viewFinderThread;

signals:
    void stateChanged(QGPhotoCaptureSession::State state);
    void error(int error, const QString &errorString);

    void imageExposed(int id);
    void imageCaptured(int id, const QImage &preview);
    void imageMetadataAvailable(int id, const QString &key, const QVariant &value);
    void imageAvailable(int id, const QVideoFrame &buffer);
    void imageSaved(int id, const QString &fileName);

    void captureError(int id, int error, const QString &errorString);


public slots:
    void videoSurfaceActive(bool active);
    void previewAvailable(const QImage &preview);
};


#endif // QGPHOTOCAPTURESESSION_H
