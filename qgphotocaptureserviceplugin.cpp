#include "qgphotocaptureserviceplugin.h"

#include <gphoto2/gphoto2-context.h>
#include <gphoto2/gphoto2-camera.h>

#include <QtCore/qdebug.h>
#include <QtCore/QDebug>

#include <iostream>

#include "qgphotocaptureservice.h"

QMediaService* QGPhotoCaptureServicePlugin::create(const QString &key)
{
    std::cout << "Hello World\n";
    std::cout.flush();

    if (key == QLatin1String(Q_MEDIASERVICE_CAMERA))
        return new QGPhotoCaptureService(key);

    qWarning() << "GPhoto capture service plugin: unsupported key:" << key;

    return 0;
}

void QGPhotoCaptureServicePlugin::release(QMediaService *service)
{
    if (service)
        delete service;
}


QList<QByteArray> QGPhotoCaptureServicePlugin::devices(const QByteArray &service) const
{
    std::cout << "Devices\n";
    std::cout.flush();
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        return m_cameraDevices;
    }

    return QList<QByteArray>();
}

void QGPhotoCaptureServicePlugin::updateDevices() const
{
    m_cameraDevices.clear();
    m_cameraDescriptions.clear();


    int count;
    CameraList *list;
    const char *name = NULL, *value = NULL;

    GPContext* gp_context = gp_context_new();

    gp_list_new (&list);
    count = gp_camera_autodetect(list, gp_context);

    for (int i = 0; i < count; i ++) {
        gp_list_get_name  (list, i, &name);
        gp_list_get_value (list, i, &value);

        m_cameraDevices.append(QByteArray(value));
        m_cameraDescriptions.append(name);
    }

    gp_list_free(list);
    gp_context_unref(gp_context);
}

QString QGPhotoCaptureServicePlugin::deviceDescription(const QByteArray &service, const QByteArray &device)
{
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        for (int i=0; i<m_cameraDevices.count(); i++)
            if (m_cameraDevices[i] == device)
                return m_cameraDescriptions[i];
    }

    return QString();
}



#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(QDSLRCamera, QGPhotoCaptureServicePlugin)
#endif // QT_VERSION < 0x050000
