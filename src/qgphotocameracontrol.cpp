#include "qgphotocameracontrol.h"

#include <QtCore/QDebug>


QGPhotoCameraControl::QGPhotoCameraControl(QGPhotoCaptureSession *captureSession) :
    QCameraControl(captureSession),
    m_captureSession(captureSession),
    m_state(QCamera::UnloadedState),
    m_status(QCamera::UnloadedStatus),
    m_captureMode(QCamera::CaptureStillImage)
{
    connect(m_captureSession, SIGNAL(stateChanged(QGPhotoCaptureSession::State)),
            this, SLOT(updateStatus()));
}

void QGPhotoCameraControl::updateStatus() {
    qDebug() << "Update status";
    QCamera::Status oldStatus = m_status;

    switch (m_state) {
    case QCamera::UnloadedState:
        m_status = QCamera::UnloadedStatus;
        break;
    case QCamera::LoadedState:
        m_status = QCamera::LoadedStatus;
        break;
    case QCamera::ActiveState:
        QGPhotoCaptureSession::State state = m_captureSession->state();

        if (state == QGPhotoCaptureSession::StoppedState)
            m_status = QCamera::StartingStatus;
        else if (state == QGPhotoCaptureSession::ErrorState) {
            m_status = QCamera::UnavailableStatus;
        } else {
           m_status = QCamera::ActiveStatus;
        }

        break;
    }

    if (oldStatus != m_status) {
        qDebug() << "Status changed:" << m_status;
        emit statusChanged(m_status);
    }
}

QCamera::State QGPhotoCameraControl::state() const {

    // TODO
    qDebug() << "QGPhotoCameraControl::state";

    return QCamera::LoadedState;
}

void QGPhotoCameraControl::setState(QCamera::State state) {
    qDebug() << "QGPhotoCameraControl::setState";

    if (m_state == state)
        return;

    m_state = state;
    switch (state) {
    case QCamera::UnloadedState:
    case QCamera::LoadedState:
        // TODO m_session->setState(QGstreamerCaptureSession::StoppedState);
        break;
    case QCamera::ActiveState:
        m_captureSession->openDevice();
        break;
    default:
        emit error(QCamera::NotSupportedFeatureError, tr("State not supported."));
    }

    updateStatus();
    emit stateChanged(m_state);
}

QCamera::Status QGPhotoCameraControl::status() const {
    qDebug() << "QGPhotoCameraControl::status";

    return m_status;
}

QCamera::CaptureModes QGPhotoCameraControl::captureMode() const {
    qDebug() << "QGPhotoCameraControl::captureMode";

    return m_captureMode;
}

void QGPhotoCameraControl::setCaptureMode(QCamera::CaptureModes captureMode) {
    qDebug() << "QGPhotoCameraControl::setCaptureMode";

    m_captureMode = captureMode;
    // TODO set capture mode on session
}

bool QGPhotoCameraControl::isCaptureModeSupported(QCamera::CaptureModes mode) const {
    qDebug() << "QGPhotoCameraControl::isCaptureModeSupported";

    // TODO check supported capture mode on session
    return true;
}

bool QGPhotoCameraControl::canChangeProperty(PropertyChangeType changeType, QCamera::Status status) const {
    qDebug() << "QGPhotoCameraControl::canChangeProperty";

    Q_UNUSED(status);

    // TODO check abilities on camera

    switch (changeType) {
    case QCameraControl::CaptureMode:
    case QCameraControl::ImageEncodingSettings:
    case QCameraControl::VideoEncodingSettings:
    case QCameraControl::Viewfinder:
        return true;
    default:
        return false;
    }
}
