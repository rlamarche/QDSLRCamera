#include "qgphotocapturesession.h"

#include <gphoto2/gphoto2-camera.h>

QGPhotoCaptureSession::QGPhotoCaptureSession(QObject *parent) :
    QObject(parent)
{
    m_gpContext = gp_context_new();
}

QGPhotoCaptureSession::~QGPhotoCaptureSession()
{
    gp_context_unref(m_gpContext);
}


void QGPhotoCaptureSession::updateDevices() const
{
    m_cameraDevices.clear();
    m_cameraDescriptions.clear();


    int count;
    CameraList *list;
    const char *name = NULL, *value = NULL;

    gp_list_new (&list);
    count = gp_camera_autodetect(list, m_gpContext);

    for (int i = 0; i < count; i ++) {
        gp_list_get_name  (list, i, &name);
        gp_list_get_value (list, i, &value);

        m_cameraDevices.append(QByteArray(value));
        m_cameraDescriptions.append(name);
    }

    gp_list_free(list);
}
