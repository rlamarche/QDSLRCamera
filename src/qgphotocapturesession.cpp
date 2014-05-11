#include "qgphotocapturesession.h"

#include <gphoto2/gphoto2-port-info-list.h>
#include <QtCore/QDebug>
#include <QFile>
#include <QVideoSurfaceFormat>

#include "qgphotoviewfinderthread.h"

QGPhotoCaptureSession::QGPhotoCaptureSession(QObject *parent) :
    QObject(parent),
    m_deviceChanged(false),
    m_videoSurface(0)
{
    m_gpContext = gp_context_new();
    m_gpCamera = 0;
    m_gpWindow = 0;
    updateDevices();

    m_selectedDevice = -1;
    m_state = QGPhotoCaptureSession::StoppedState;

    m_viewFinderThread = new QGPhotoViewFinderThread(this);
    connect(m_viewFinderThread, SIGNAL(previewAvailable(QImage)), this, SLOT(previewAvailable(QImage)));
}

QGPhotoCaptureSession::~QGPhotoCaptureSession()
{
    if (m_viewFinderThread->isRunning()) {
        m_viewFinderThread->stopNow();
    }
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

void QGPhotoCaptureSession::openDevice() {
    if (!m_gpCamera || m_deviceChanged) {
        if (m_gpCamera) {
            closeDevice();
        }

        int gpStatus;
        // the port handle
        int portHandle;

        // model index
        int gpModelIndex;
        GPPortInfoList *gpPortInfoList;
        GPPortInfo gpPortInfo;
        CameraAbilitiesList *gpCameraAbilitiesList;
        CameraAbilities gpCameraAbilities;
    /*
        CameraEventType gpEvtType;
        void *gpEvtData;
    */
        // Create a camera object
        gpStatus = gp_camera_new(&m_gpCamera);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to create camera object"));
            return;
        }

        // Lookup camera model
        gpStatus = gp_abilities_list_new(&gpCameraAbilitiesList);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to create abilities list"));
            return;
        }

        gpStatus = gp_abilities_list_load(gpCameraAbilitiesList, m_gpContext);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to load abilities list"));
            return;
        }

        gpModelIndex = gp_abilities_list_lookup_model(gpCameraAbilitiesList, m_cameraDescriptions[m_selectedDevice].toStdString().c_str());
        if (gpModelIndex < 0) {
            gpError(gpStatus, QString("Unable to lookup model"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            return;
        }

        gpStatus = gp_abilities_list_get_abilities(gpCameraAbilitiesList, gpModelIndex, &gpCameraAbilities);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to get camera model abilities"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            return;
        }

        gpStatus = gp_camera_set_abilities (m_gpCamera, gpCameraAbilities);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to set abilities"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            return;
        }

        // Get usb port
        gpStatus = gp_port_info_list_new(&gpPortInfoList);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to get usb port"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            return;
        }

        gpStatus = gp_port_info_list_load(gpPortInfoList);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to load port info list"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            gp_port_info_list_free(gpPortInfoList);
            return;
        }

        portHandle = gp_port_info_list_lookup_path (gpPortInfoList, QString(m_cameraDevices[m_selectedDevice]).toStdString().c_str());
        if (portHandle < 0) {
            gpError(gpStatus, QString("Unable to lookup port path"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            gp_port_info_list_free(gpPortInfoList);
            return;
        }

        gpStatus = gp_port_info_list_get_info (gpPortInfoList, portHandle, &gpPortInfo);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to get port info list"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            gp_port_info_list_free(gpPortInfoList);
            return;
        }

        gpStatus = gp_camera_set_port_info (m_gpCamera, gpPortInfo);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to set port info"));
            gp_abilities_list_free(gpCameraAbilitiesList);
            gp_port_info_list_free(gpPortInfoList);
            return;
        }

        // Useless function ?
        gp_camera_init(m_gpCamera, m_gpContext);

        gp_abilities_list_free(gpCameraAbilitiesList);
        gp_port_info_list_free(gpPortInfoList);

        gpStatus = gp_camera_get_config(m_gpCamera, &m_gpWindow, m_gpContext);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to get camera config"));
            return;
        }
    }
    m_state = QGPhotoCaptureSession::ReadyState;
    emit stateChanged(m_state);
    m_deviceChanged = false;
}

void QGPhotoCaptureSession::closeDevice() {
    qDebug("QGPhotoCaptureSession::closeDevice");
    stopViewFinder();

    if (m_gpWindow) {
        gp_widget_free(m_gpWindow);
        m_gpWindow = 0;
    }
    if (m_gpCamera) {
        gp_camera_exit(m_gpCamera, m_gpContext);
        gp_camera_unref(m_gpCamera);
        m_gpCamera = 0;
    }
    m_state = StoppedState;
}

void QGPhotoCaptureSession::captureImage(int reqId, const QString &fileName) {
    CameraFile *gpFile;
    CameraFilePath gpCameraFilePath;
    int gpStatus;

    if (!fileName.isEmpty()) {
        gpStatus = gp_camera_capture(m_gpCamera, GP_CAPTURE_IMAGE, &gpCameraFilePath, m_gpContext);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to capture image"));
            return;
        }

        qDebug("Pathname on the camera: %s/%s\n", gpCameraFilePath.folder, gpCameraFilePath.name);

        QFile output(fileName);
        output.open(QIODevice::WriteOnly);

        gpStatus = gp_file_new_from_fd(&gpFile, output.handle());
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to create file from fd"));
            return;
        }

        gpStatus = gp_camera_file_get(m_gpCamera, gpCameraFilePath.folder, gpCameraFilePath.name,
                 GP_FILE_TYPE_NORMAL, gpFile, m_gpContext);
        if (gpStatus != GP_OK) {
            gp_file_free(gpFile);

            gpError(gpStatus, QString("Unable to get file from camera"));
            return;
        }

        qDebug() << "Delete file on camera";

        gpStatus = gp_camera_file_delete(m_gpCamera, gpCameraFilePath.folder, gpCameraFilePath.name,
                m_gpContext);
        if (gpStatus != GP_OK) {
            gp_file_free(gpFile);

            gpError(gpStatus, QString("Unable to delete file"));
            return;
        }

        output.close();

        gp_file_free(gpFile);
    }
}

int QGPhotoCaptureSession::deviceCount() const {
    return m_cameraDevices.length();
}

QString QGPhotoCaptureSession::deviceName(int index) const {
    return m_cameraDevices[index];
}

QString QGPhotoCaptureSession::deviceDescription(int index) const {
    return m_cameraDescriptions[index];
}

int QGPhotoCaptureSession::defaultDevice() const {
    return 0;
}

int QGPhotoCaptureSession::selectedDevice() const {
    return m_selectedDevice;
}

void QGPhotoCaptureSession::setSelectedDevice(int index) {
    if (m_state == ReadyState && index != m_selectedDevice) {
        qDebug("Selected device has changed !");
        m_deviceChanged = true;
    }
    m_selectedDevice = index;
}

void QGPhotoCaptureSession::gpError(int gpStatus, QString message) {
    QString errorMsg;

    switch (gpStatus) {
        case GP_OK:
            errorMsg = QObject::tr("No error");
        case GP_ERROR:
            errorMsg = QObject::tr("Unspecified error");
        case GP_ERROR_IO:
            errorMsg = QObject::tr("I/O problem");
        case GP_ERROR_BAD_PARAMETERS:
            errorMsg = QObject::tr("Bad parameters");
        case GP_ERROR_NOT_SUPPORTED:
            errorMsg = QObject::tr("Unsupported operation");
        case  GP_ERROR_FIXED_LIMIT_EXCEEDED:
            errorMsg = QObject::tr("Fixed limit exceeded");
        case GP_ERROR_TIMEOUT:
            errorMsg = QObject::tr("Timeout reading from or writing to the port");
        case GP_ERROR_IO_SUPPORTED_SERIAL:
            errorMsg = QObject::tr("Serial port not supported");
        case GP_ERROR_IO_SUPPORTED_USB:
            errorMsg = QObject::tr("USB port not supported");
        case GP_ERROR_UNKNOWN_PORT:
            errorMsg = QObject::tr("Unknown port");
        case GP_ERROR_NO_MEMORY:
            errorMsg = QObject::tr("Out of memory");
        case GP_ERROR_LIBRARY:
            errorMsg = QObject::tr("Error loading a library");
        case GP_ERROR_IO_INIT:
            errorMsg = QObject::tr("Error initializing the port");
        case GP_ERROR_IO_READ:
            errorMsg = QObject::tr("Error reading from the port");
        case GP_ERROR_IO_WRITE:
            errorMsg = QObject::tr("Error writing to the port");
        case GP_ERROR_IO_UPDATE:
            errorMsg = QObject::tr("Error updating the port settings");
        case GP_ERROR_IO_SERIAL_SPEED:
            errorMsg = QObject::tr("Error setting the serial port speed");
        case GP_ERROR_IO_USB_CLEAR_HALT:
            errorMsg = QObject::tr("Error clearing a halt condition on the USB port");
        case GP_ERROR_IO_USB_FIND:
            errorMsg = QObject::tr("Could not find the requested device on the USB port");
        case GP_ERROR_IO_USB_CLAIM:
            errorMsg = QObject::tr("Could not claim the USB device");
        case GP_ERROR_IO_LOCK:
            errorMsg = QObject::tr("Could not lock the device");
        case GP_ERROR_HAL:
            errorMsg = QObject::tr("libhal error");
        case GP_ERROR_CORRUPTED_DATA:
            errorMsg = QObject::tr("Corrupted data received");
        case GP_ERROR_FILE_EXISTS:
            errorMsg = QObject::tr("File already exists");
        case GP_ERROR_MODEL_NOT_FOUND:
            errorMsg = QObject::tr("Specified camera model was not found");
        case GP_ERROR_DIRECTORY_NOT_FOUND:
            errorMsg = QObject::tr("Specified directory was not found")        ;
        case GP_ERROR_FILE_NOT_FOUND:
            errorMsg = QObject::tr("Specified directory was not found");
        case GP_ERROR_DIRECTORY_EXISTS:
            errorMsg = QObject::tr("Specified directory already exists");
        case GP_ERROR_CAMERA_BUSY:
            errorMsg = QObject::tr("The camera is already busy");
        case GP_ERROR_PATH_NOT_ABSOLUTE:
            errorMsg = QObject::tr("Path is not absolute");
        case GP_ERROR_CANCEL:
            errorMsg = QObject::tr("Cancellation successful");
        case GP_ERROR_CAMERA_ERROR:
            errorMsg = QObject::tr("Unspecified camera error");
        case GP_ERROR_OS_FAILURE:
            errorMsg = QObject::tr("Unspecified failure of the operating system");
        case GP_ERROR_NO_SPACE:
            errorMsg = QObject::tr("Not enough space");
        default:
            errorMsg = QObject::tr("Unknown error %1").arg(QString().sprintf("%d", gpStatus));
    }

    qWarning() << "GPhoto error : " << message << " : " << errorMsg;

    m_state = QGPhotoCaptureSession::ErrorState;
    emit stateChanged(m_state);
    emit error(gpStatus, errorMsg);
}

QGPhotoCaptureSession::State QGPhotoCaptureSession::state() {
    return m_state;
}

void QGPhotoCaptureSession::setSurface(QAbstractVideoSurface *surface) {
    qDebug("QGPhotoCaptureSession::setSurface");
    if (m_videoSurface && m_videoSurface != surface) {
        disconnect(m_videoSurface, SIGNAL(activeChanged(bool)), this, SLOT(videoSurfaceActive(bool)));
    }
    m_videoSurface = surface;

    connect(m_videoSurface, SIGNAL(activeChanged(bool)), this, SLOT(videoSurfaceActive(bool)));
}

QImage QGPhotoCaptureSession::capturePreview() {
    int gpStatus;
    CameraFile *gpFile;
    unsigned long int size;
    const char *data;
    QImage image;

    //CameraEventType evttype;;
    //void *evtdata;

    gpStatus = gp_file_new(&gpFile);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to create file for capture"));
        return image;
    }

    gpStatus = gp_camera_capture_preview(m_gpCamera, gpFile, m_gpContext);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to capture preview"));
        gp_file_free(gpFile);
        return image;
    }

    gpStatus = gp_file_get_data_and_size(gpFile, &data, &size);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get file data and size"));
        gp_file_free(gpFile);
        return image;
    }

    image.loadFromData((uchar*) data, size, "JPG");

    gp_file_free(gpFile);
    return image;
}

void QGPhotoCaptureSession::videoSurfaceActive(bool active) {
    if (active && m_videoSurface) {
        qDebug("Surface active");
        m_viewFinderThread->start();
    }
}

void QGPhotoCaptureSession::previewAvailable(const QImage &preview) {
    if (m_videoSurface->isActive()) {
        QVideoFrame frame(preview);
        m_videoSurface->present(frame);
    }
}

void QGPhotoCaptureSession::startViewFinder() {
    if (m_videoSurface) {
        int value = 1;
        setWidgetValue(QString("viewfinder"), &value);
        QImage preview = capturePreview();
        QVideoFrame frame(preview);

        QVideoSurfaceFormat videoSurfaceFormat(frame.size(), frame.pixelFormat());
        m_videoSurface->start(videoSurfaceFormat);
    }
}

void QGPhotoCaptureSession::stopViewFinder() {
    if (m_videoSurface) {
        m_videoSurface->stop();
    }

    m_viewFinderThread->stopNow();
    if (m_gpContext && m_gpCamera && m_gpWindow) {
        int value = 0;
        setWidgetValue(QString("viewfinder"), &value);
    }
}

void QGPhotoCaptureSession::setWidgetValue(QString name, const void *value) {
    CameraWidget *gpWidget;
    int gpStatus;

    gpStatus = gp_widget_get_child_by_name(m_gpWindow, name.toStdString().c_str(), &gpWidget);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get child widget by name"));
        return;
    }

    gpStatus = gp_widget_set_value(gpWidget, value);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to set widget value"));
        return;
    }

    gpStatus = gp_camera_set_config(m_gpCamera, m_gpWindow, m_gpContext);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to set config"));
        return;
    }
}
