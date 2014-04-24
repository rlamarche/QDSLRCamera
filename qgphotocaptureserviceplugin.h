#ifndef QGPHOTOCAPTURESERVICEPLUGIN_H
#define QGPHOTOCAPTURESERVICEPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QByteArray>

#include <QMediaServiceProviderPlugin>

class QGPhotoCaptureServicePlugin : public QMediaServiceProviderPlugin
        , public QMediaServiceSupportedDevicesInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaServiceSupportedDevicesInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.mediaserviceproviderfactory/5.0" FILE "QDSLRCamera.json")
#endif // QT_VERSION >= 0x050000
public:
    QMediaService* create(QString const& key);
    void release(QMediaService *service);

    QList<QByteArray> devices(const QByteArray &service) const;
    QString deviceDescription(const QByteArray &service, const QByteArray &device);

private:
    void updateDevices() const;

    mutable QList<QByteArray> m_cameraDevices;
    mutable QStringList m_cameraDescriptions;

signals:

public slots:

};

#endif // QGPHOTOCAPTURESERVICEPLUGIN_H
