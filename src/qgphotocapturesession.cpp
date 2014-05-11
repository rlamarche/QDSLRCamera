#include "qgphotocapturesession.h"

#include <gphoto2/gphoto2-port-info-list.h>
#include <QtCore/QDebug>
#include <QFile>
#include <QVideoSurfaceFormat>
#include <QDir>
#include <QRegExp>

#include "qgphotoviewfinderthread.h"

QGPhotoCaptureSession::QGPhotoCaptureSession(QObject *parent) :
    QObject(parent),
    m_viewFinderRunning(false),
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

void QGPhotoCaptureSession::openDevice()
{
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

void QGPhotoCaptureSession::closeDevice()
{
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

void QGPhotoCaptureSession::captureImage(int reqId, const QString &fileName)
{
    CameraFile *gpFile;
    CameraFilePath gpCameraFilePath;
    int gpStatus;
    CameraEventType gpEvtType;
    void *gpEvtData;

    bool viewFinderRunning = m_viewFinderRunning;

    if (m_viewFinderRunning) {
        pauseViewFinder();
    }

    QString path = fileName;
    if (path.isEmpty()) {
        int lastImage = 0;
        QDir outputDir = QDir::currentPath();
        foreach(QString fileName, outputDir.entryList(QStringList() << "img_*.jpg")) {
            int imgNumber = fileName.mid(4, fileName.size()-8).toInt();
            lastImage = qMax(lastImage, imgNumber);
        }

        path = QString("img_%1.jpg").arg(lastImage+1,
                                         4, //fieldWidth
                                         10,
                                         QLatin1Char('0'));
    }


    gpStatus = gp_camera_capture(m_gpCamera, GP_CAPTURE_IMAGE, &gpCameraFilePath, m_gpContext);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to capture image"));
        emit captureError(reqId, gpStatus, QString("Unable to capture image"));

        return;
    }
    emit imageExposed(reqId);

   // gp_camera_wait_for_event(m_gpCamera, -1, &gpEvtType, &gpEvtData, m_gpContext);
/*
    do {
        gpStatus = gp_camera_wait_for_event(m_gpCamera, 5000, &gpEvtType, &gpEvtData, m_gpContext);
    } while (gpStatus == GP_OK && (gpEvtType != GP_EVENT_CAPTURE_COMPLETE || gpEvtType != GP_EVENT_TIMEOUT));
*/
   /* if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to capture image"));
        emit captureError(reqId, gpStatus, QString("Error waiting end capture image"));

        return;
    }*/

    qDebug("Pathname on the camera: %s/%s\n", gpCameraFilePath.folder, gpCameraFilePath.name);

    QFile output(path);
    output.open(QIODevice::WriteOnly);

    gpStatus = gp_file_new_from_fd(&gpFile, output.handle());
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to create file from fd"));
        emit captureError(reqId, gpStatus, QString("Unable to create file from fd"));
        return;
    }

    gpStatus = gp_camera_file_get(m_gpCamera, gpCameraFilePath.folder, gpCameraFilePath.name,
             GP_FILE_TYPE_NORMAL, gpFile, m_gpContext);
    if (gpStatus != GP_OK) {
        gp_file_free(gpFile);

        gpError(gpStatus, QString("Unable to get file from camera"));
        emit captureError(reqId, gpStatus, QString("Unable to get file from camera"));

        return;
    }
    emit imageSaved(reqId, path);

    qDebug() << "Delete file on camera";

    gpStatus = gp_camera_file_delete(m_gpCamera, gpCameraFilePath.folder, gpCameraFilePath.name,
            m_gpContext);
    if (gpStatus != GP_OK) {
        gp_file_free(gpFile);

        gpError(gpStatus, QString("Unable to delete file"));
        emit captureError(reqId, gpStatus, QString("Unable to delete file"));
        return;
    }

    output.close();

    gp_file_free(gpFile);

    gp_camera_wait_for_event(m_gpCamera, -1, &gpEvtType, &gpEvtData, m_gpContext);

    if (viewFinderRunning) {
        continueViewFinder();
    }

}

int QGPhotoCaptureSession::deviceCount() const
{
    return m_cameraDevices.length();
}

QString QGPhotoCaptureSession::deviceName(int index) const
{
    return m_cameraDevices[index];
}

QString QGPhotoCaptureSession::deviceDescription(int index) const
{
    return m_cameraDescriptions[index];
}

int QGPhotoCaptureSession::defaultDevice() const
{
    return 0;
}

int QGPhotoCaptureSession::selectedDevice() const
{
    return m_selectedDevice;
}

void QGPhotoCaptureSession::setSelectedDevice(int index)
{
    if (m_state == ReadyState && index != m_selectedDevice) {
        qDebug("Selected device has changed !");
        m_deviceChanged = true;
    }
    m_selectedDevice = index;
}

void QGPhotoCaptureSession::gpError(int gpStatus, QString message)
{
    QString errorMsg;

    switch (gpStatus) {
        case GP_OK:
            errorMsg = QObject::tr("No error");
            break;
        case GP_ERROR:
            errorMsg = QObject::tr("Unspecified error");
            break;
        case GP_ERROR_IO:
            errorMsg = QObject::tr("I/O problem");
            break;
        case GP_ERROR_BAD_PARAMETERS:
            errorMsg = QObject::tr("Bad parameters");
            break;
        case GP_ERROR_NOT_SUPPORTED:
            errorMsg = QObject::tr("Unsupported operation");
            break;
        case  GP_ERROR_FIXED_LIMIT_EXCEEDED:
            errorMsg = QObject::tr("Fixed limit exceeded");
            break;
        case GP_ERROR_TIMEOUT:
            errorMsg = QObject::tr("Timeout reading from or writing to the port");
            break;
        case GP_ERROR_IO_SUPPORTED_SERIAL:
            errorMsg = QObject::tr("Serial port not supported");
            break;
        case GP_ERROR_IO_SUPPORTED_USB:
            errorMsg = QObject::tr("USB port not supported");
            break;
        case GP_ERROR_UNKNOWN_PORT:
            errorMsg = QObject::tr("Unknown port");
            break;
        case GP_ERROR_NO_MEMORY:
            errorMsg = QObject::tr("Out of memory");
            break;
        case GP_ERROR_LIBRARY:
            errorMsg = QObject::tr("Error loading a library");
            break;
        case GP_ERROR_IO_INIT:
            errorMsg = QObject::tr("Error initializing the port");
            break;
        case GP_ERROR_IO_READ:
            errorMsg = QObject::tr("Error reading from the port");
            break;
        case GP_ERROR_IO_WRITE:
            errorMsg = QObject::tr("Error writing to the port");
            break;
        case GP_ERROR_IO_UPDATE:
            errorMsg = QObject::tr("Error updating the port settings");
            break;
        case GP_ERROR_IO_SERIAL_SPEED:
            errorMsg = QObject::tr("Error setting the serial port speed");
            break;
        case GP_ERROR_IO_USB_CLEAR_HALT:
            errorMsg = QObject::tr("Error clearing a halt condition on the USB port");
            break;
        case GP_ERROR_IO_USB_FIND:
            errorMsg = QObject::tr("Could not find the requested device on the USB port");
            break;
        case GP_ERROR_IO_USB_CLAIM:
            errorMsg = QObject::tr("Could not claim the USB device");
            break;
        case GP_ERROR_IO_LOCK:
            errorMsg = QObject::tr("Could not lock the device");
            break;
        case GP_ERROR_HAL:
            errorMsg = QObject::tr("libhal error");
            break;
        case GP_ERROR_CORRUPTED_DATA:
            errorMsg = QObject::tr("Corrupted data received");
            break;
        case GP_ERROR_FILE_EXISTS:
            errorMsg = QObject::tr("File already exists");
            break;
        case GP_ERROR_MODEL_NOT_FOUND:
            errorMsg = QObject::tr("Specified camera model was not found");
            break;
        case GP_ERROR_DIRECTORY_NOT_FOUND:
            errorMsg = QObject::tr("Specified directory was not found")        ;
            break;
        case GP_ERROR_FILE_NOT_FOUND:
            errorMsg = QObject::tr("Specified directory was not found");
            break;
        case GP_ERROR_DIRECTORY_EXISTS:
            errorMsg = QObject::tr("Specified directory already exists");
            break;
        case GP_ERROR_CAMERA_BUSY:
            errorMsg = QObject::tr("The camera is already busy");
            break;
        case GP_ERROR_PATH_NOT_ABSOLUTE:
            errorMsg = QObject::tr("Path is not absolute");
            break;
        case GP_ERROR_CANCEL:
            errorMsg = QObject::tr("Cancellation successful");
            break;
        case GP_ERROR_CAMERA_ERROR:
            errorMsg = QObject::tr("Unspecified camera error");
            break;
        case GP_ERROR_OS_FAILURE:
            errorMsg = QObject::tr("Unspecified failure of the operating system");
            break;
        case GP_ERROR_NO_SPACE:
            errorMsg = QObject::tr("Not enough space");
            break;
        default:
            errorMsg = QObject::tr("Unknown error %1").arg(QString().sprintf("%d", gpStatus));
            break;
    }

    qWarning("GPhoto error : %s : %s", message.toStdString().c_str(), errorMsg.toStdString().c_str());

    m_state = QGPhotoCaptureSession::ErrorState;
    emit stateChanged(m_state);
    emit error(gpStatus, errorMsg);
}

QGPhotoCaptureSession::State QGPhotoCaptureSession::state()
{
    return m_state;
}

void QGPhotoCaptureSession::setSurface(QAbstractVideoSurface *surface)
{
    qDebug("QGPhotoCaptureSession::setSurface");
    if (m_videoSurface && m_videoSurface != surface) {
        disconnect(m_videoSurface, SIGNAL(activeChanged(bool)), this, SLOT(videoSurfaceActive(bool)));
    }
    m_videoSurface = surface;

    connect(m_videoSurface, SIGNAL(activeChanged(bool)), this, SLOT(videoSurfaceActive(bool)));
}

QImage QGPhotoCaptureSession::capturePreview()
{
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

void QGPhotoCaptureSession::videoSurfaceActive(bool active)
{
    if (active && m_videoSurface) {
        qDebug("Surface active");
        m_viewFinderThread->start();
    }
}

void QGPhotoCaptureSession::previewAvailable(const QImage &preview)
{
    if (m_videoSurface->isActive()) {
        QVideoFrame frame(preview);
        m_videoSurface->present(frame);
    }
}

void QGPhotoCaptureSession::startViewFinder()
{
    if (m_videoSurface) {
        int value = 1;
        setWidgetValue(QString("viewfinder"), QVariant(value));
        QImage preview = capturePreview();
        QVideoFrame frame(preview);

        QVideoSurfaceFormat videoSurfaceFormat(frame.size(), frame.pixelFormat());
        m_videoSurface->start(videoSurfaceFormat);
        m_viewFinderRunning = true;
    }
}

void QGPhotoCaptureSession::pauseViewFinder()
{
    m_viewFinderThread->stopNow();
    if (m_gpContext && m_gpCamera && m_gpWindow) {
        int value = 0;
        setWidgetValue(QString("viewfinder"), QVariant(value));
    }
    m_viewFinderRunning = false;
}

void QGPhotoCaptureSession::continueViewFinder()
{
    int value = 1;
    setWidgetValue(QString("viewfinder"), QVariant(value));
    m_viewFinderThread->start();
    m_viewFinderRunning = true;
}

void QGPhotoCaptureSession::stopViewFinder()
{
    if (m_videoSurface) {
        m_videoSurface->stop();
    }

    m_viewFinderThread->stopNow();
    if (m_gpContext && m_gpCamera && m_gpWindow) {
        int value = 0;
        setWidgetValue(QString("viewfinder"), QVariant(value));
    }
    m_viewFinderRunning = false;
}

void QGPhotoCaptureSession::setWidgetValue(QString name, QVariant value)
{
    CameraWidget *gpWidget;
    CameraWidgetType gpWidgetType;
    int gpStatus;
    int intValue;
    float floatValue;

    gpStatus = gp_widget_get_child_by_name(m_gpWindow, name.toStdString().c_str(), &gpWidget);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get child widget by name"));
        return;
    }

    gpStatus = gp_widget_get_type(gpWidget, &gpWidgetType);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get widget type"));
        return;
    }

    switch (gpWidgetType) {
        case GP_WIDGET_RADIO:
        case GP_WIDGET_MENU:
        case GP_WIDGET_TEXT:
            gp_widget_set_value(gpWidget, value.toString().toStdString().c_str());
            break;

        case GP_WIDGET_TOGGLE:
        case GP_WIDGET_DATE:
            intValue = value.toInt();
            gpStatus = gp_widget_set_value(gpWidget, &intValue);
            break;
        case GP_WIDGET_RANGE:
            floatValue = value.toFloat();
            gpStatus = gp_widget_set_value(gpWidget, &floatValue);
            break;

        default: break;
    }

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

QVariantList QGPhotoCaptureSession::getWidgetRange(QString name)
{
    CameraWidget *gpWidget;
    int gpStatus;
    QVariantList res;
    const char* choiceLabel;

    gpStatus = gp_widget_get_child_by_name(m_gpWindow, name.toStdString().c_str(), &gpWidget);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get child widget by name"));
        return res;
    }

    int n = gp_widget_count_choices(gpWidget);


    for (int i = 0; i < n; i ++) {
        gpStatus = gp_widget_get_choice(gpWidget, i, &choiceLabel);
        if (gpStatus != GP_OK) {
            gpError(gpStatus, QString("Unable to get widget choice"));
            break;
        }
        res << QString(choiceLabel);
    }

    return res;
}

void QGPhotoCaptureSession::getWidgetValue(QString name, void *value)
{
    CameraWidget *gpWidget;
    int gpStatus;

    gpStatus = gp_camera_get_config(m_gpCamera, &m_gpWindow, m_gpContext);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get camera config"));
        return;
    }

    gpStatus = gp_widget_get_child_by_name(m_gpWindow, name.toStdString().c_str(), &gpWidget);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get child widget by name"));
    }

    gpStatus = gp_widget_get_value(gpWidget, value);
    if (gpStatus != GP_OK) {
        gpError(gpStatus, QString("Unable to get widget value"));
    }
}

QVariant QGPhotoCaptureSession::actualExposureValue(QCameraExposureControl::ExposureParameter parameter)
{
    QString name = exposureParameterToWidgetName(parameter);

    char* value = 0;

    getWidgetValue(name, &value);

    if (value) {
        return exposureLabelToValue(parameter, QVariant(QString(value)));
    } else {
        return QVariant::Invalid;
    }
}

bool QGPhotoCaptureSession::setExposureValue(QCameraExposureControl::ExposureParameter parameter, const QVariant &value)
{
    QVariantList supportedValues = supportedExposureParameterRange(parameter, 0);

    bool ok;
    qreal realValue = value.toReal(&ok);
    if (!ok)
    {
        return false;
    }

    // Find the nearest value always rounded below
    int i = 0;
    qreal current;
    for (QVariantList::Iterator iter = supportedValues.begin(); iter != supportedValues.end(); iter ++) {
        current = (*iter).toReal();

        if (realValue == current) {
            break;
        }

        if (iter != supportedValues.end() - 1) {
            qreal next = (*(iter + 1)).toReal();

            if (realValue > current && realValue < next) {
                break;
            }
            if (realValue < current && realValue > next) {
                i ++;
                break;
            }
        }
        i ++;
    }

    QString name = exposureParameterToWidgetName(parameter);
    QVariantList strValues = getWidgetRange(name);

    setWidgetValue(name, strValues.at(i));

    return true;
}

QVariantList QGPhotoCaptureSession::supportedExposureParameterRange(QCameraExposureControl::ExposureParameter parameter, bool *continuous)
{
    if (continuous) {
        *continuous = false;
    }

    QString widgetName = exposureParameterToWidgetName(parameter);

    QVariantList values = getWidgetRange(widgetName);
    QVariantList qrealValues;
    for (QVariantList::Iterator value = values.begin(); value != values.end(); value ++) {
        qrealValues << exposureLabelToValue(parameter, *value);
    }

    return qrealValues;
}

QString QGPhotoCaptureSession::exposureParameterToWidgetName(QCameraExposureControl::ExposureParameter parameter) const
{
    switch (parameter) {
    case QCameraExposureControl::ExposureCompensation:
        return QString("exposurecompensation");
        break;
    case QCameraExposureControl::ISO:
        return QString("iso");
        break;
    case QCameraExposureControl::Aperture:
        return QString("5007");
        break;
    case QCameraExposureControl::ShutterSpeed:
        return QString("500d");
        break;
    default:
        break;
    }

    return QString("unknown");
}

QVariant QGPhotoCaptureSession::exposureLabelToValue(QCameraExposureControl::ExposureParameter parameter, QVariant value) const
{
    switch (parameter) {
    case QCameraExposureControl::ISO:
        return value;
        break;
    case QCameraExposureControl::Aperture:
        return QVariant(value.toReal() / 100.0);
        break;
    case QCameraExposureControl::ShutterSpeed:
        return QVariant(value.toReal() / 10000.0);
        break;
    default:
        return value;
        break;
    }
}
