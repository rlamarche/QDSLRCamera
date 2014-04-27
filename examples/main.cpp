#include <QCoreApplication>


#include <QCamera>


#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<QByteArray> devices = QCamera::availableDevices();


   // QGPhotoServicePlugin s;

   // devices = s.devices(Q_MEDIASERVICE_CAMERA);


    for (QList<QByteArray>::Iterator i = devices.begin(); i != devices.end(); i ++) {
        QByteArray current = *i;

        cout << QString(current).toStdString() << "\n";
    }

    //QCamera *camera = new QCamera(devices[1]);

    cout.flush();

    //delete camera;

    return a.exec();
}
