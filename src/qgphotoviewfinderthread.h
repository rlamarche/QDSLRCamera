#ifndef QGPHOTOVIEWFINDERTHREAD_H
#define QGPHOTOVIEWFINDERTHREAD_H

#include <QThread>

#include "qgphotocapturesession.h"

class QGPhotoViewFinderThread : public QThread
{
    Q_OBJECT
public:
    explicit QGPhotoViewFinderThread(QGPhotoCaptureSession *captureSession);

    void stopNow();
protected:
    void run();
private:
    QGPhotoCaptureSession *m_captureSession;
    bool m_stop;
signals:
    void previewAvailable(const QImage &image);
public slots:

};

#endif // QGPHOTOVIEWFINDERTHREAD_H
