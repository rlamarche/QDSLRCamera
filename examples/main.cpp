#include <QCoreApplication>


#include <QCamera>
#include <QCameraImageCapture>

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

    cout << "### New camera\n";
    cout.flush();

    QCamera *camera = new QCamera(devices[1]);

    cout << "### Camera device description\n";
    cout.flush();
    QString description = camera->deviceDescription(devices.at(1)); //
    std::cout << description.toStdString() << "\n";


    QCameraImageCapture *imageCapture = new QCameraImageCapture(camera); // Object to take pictures
    camera->setCaptureMode(QCamera::CaptureStillImage); // Set the camera mode

    cout << "### Start camera\n";
    cout.flush();


    camera->start();
/*
    camera->searchAndLock();
    imageCapture->capture(QString('/tmp/test.jpg')); // Method to take a picture. Arg: path to save the picture

    camera->unlock();
*/

    imageCapture->capture(QString("/tmp/capture.nef"));

    cout << "### Stop camera\n";
    cout.flush();
    camera->stop();


    delete imageCapture;
    delete camera;

    return a.exec();
}
