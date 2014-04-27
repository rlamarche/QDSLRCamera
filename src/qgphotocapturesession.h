#ifndef QGPHOTOCAPTURESESSION_H
#define QGPHOTOCAPTURESESSION_H

#include <QObject>
#include <QList>
#include <QStringList>

#include <gphoto2/gphoto2-context.h>

class QGPhotoCaptureSession : public QObject
{
    Q_OBJECT
public:
    explicit QGPhotoCaptureSession(QObject *parent = 0);
    ~QGPhotoCaptureSession();

private:
   void updateDevices() const;

   GPContext *m_gpContext;

   mutable QList<QByteArray> m_cameraDevices;
   mutable QStringList m_cameraDescriptions;

signals:

public slots:

};

#endif // QGPHOTOCAPTURESESSION_H
