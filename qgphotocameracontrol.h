#ifndef QGPHOTOCAMERACONTROL_H
#define QGPHOTOCAMERACONTROL_H

#include <QObject>
#include <QCameraControl>

class QGPhotoCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    explicit QGPhotoCameraControl(QObject *parent = 0);

signals:

public slots:

};

#endif // QGPHOTOCAMERACONTROL_H
