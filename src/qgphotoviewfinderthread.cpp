#include "qgphotoviewfinderthread.h"

#include <QtCore/QDebug>

QGPhotoViewFinderThread::QGPhotoViewFinderThread(QGPhotoCaptureSession *captureSession) :
    QThread(captureSession),
    m_captureSession(captureSession),
    m_stop(false)
{
}

void QGPhotoViewFinderThread::stopNow() {
    m_stop = true;
    wait();
    m_stop = false;
}

void QGPhotoViewFinderThread::run() {
    forever {
        QImage preview = m_captureSession->capturePreview();
        emit previewAvailable(preview);

        if (m_stop) {
            break;
        }
    }
}
