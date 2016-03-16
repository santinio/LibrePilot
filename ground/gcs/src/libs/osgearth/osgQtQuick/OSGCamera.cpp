/**
 ******************************************************************************
 *
 * @file       OSGCamera.cpp
 * @author     The LibrePilot Project, http://www.librepilot.org Copyright (C) 2015.
 * @addtogroup
 * @{
 * @addtogroup
 * @{
 * @brief
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "OSGCamera.hpp"

#include "OSGNode.hpp"

#include "../utility.h"

#include <osg/Camera>
#include <osg/Matrix>
#include <osg/Node>
#include <osg/Vec3d>

#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>

#include <osgViewer/View>

#ifdef USE_OSGEARTH
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/LogarithmicDepthBuffer>
#endif

#include <QDebug>
#include <QThread>
#include <QApplication>

namespace osgQtQuick {
struct OSGCamera::Hidden : public QObject {
    Q_OBJECT

public:
    Hidden(OSGCamera *camera) :
        QObject(camera), self(camera), sceneNode(NULL),
        manipulatorMode(ManipulatorMode::Default), trackerMode(TrackerMode::NodeCenterAndAzim), trackNode(NULL),
        logDepthBufferEnabled(false), clampToTerrain(false), intoTerrain(false)
    {
        fieldOfView    = 90.0;

#ifdef USE_OSGEARTH
        logDepthBuffer = NULL;
#endif
    }

    ~Hidden()
    {
#ifdef USE_OSGEARTH
        if (logDepthBuffer) {
            delete logDepthBuffer;
            logDepthBuffer = NULL;
        }
#endif
    }

    bool acceptSceneNode(OSGNode *node)
    {
        qDebug() << "OSGCamera::acceptSceneNode" << node;
        if (sceneNode == node) {
            return true;
        }

        if (sceneNode) {
            disconnect(sceneNode);
        }

        sceneNode = node;

        if (sceneNode) {
            connect(sceneNode, &OSGNode::nodeChanged, this, &Hidden::onSceneNodeChanged);
        }

        return true;
    }

    bool acceptManipulatorMode(ManipulatorMode::Enum mode)
    {
        // qDebug() << "OSGCamera::acceptManipulatorMode" << mode;
        if (manipulatorMode == mode) {
            return false;
        }

        manipulatorMode = mode;

        return true;
    }

    bool acceptTrackNode(OSGNode *node)
    {
        qDebug() << "OSGCamera::acceptTrackNode" << node;
        if (trackNode == node) {
            return false;
        }

        if (trackNode) {
            disconnect(trackNode);
        }

        trackNode = node;

        if (trackNode) {
            connect(trackNode, SIGNAL(nodeChanged(osg::Node *)), this, SLOT(onTrackNodeChanged(osg::Node *)));
        }

        return true;
    }

    void attachCamera(osg::Camera *camera)
    {
        qDebug() << "OSGCamera::attach" << camera;

        this->camera = camera;
        self->setNode(this->camera.get());

#ifdef USE_OSGEARTH
        // install log depth buffer if requested
        if (logDepthBufferEnabled) {
            qDebug() << "OSGCamera::attach - install logarithmic depth buffer";
            logDepthBuffer = new osgEarth::Util::LogarithmicDepthBuffer();
            logDepthBuffer->setUseFragDepth(true);
            logDepthBuffer->install(camera);
        }
#endif

        updateFieldOfView();
        updateAspectRatio();
    }

    void detachCamera(osg::Camera *camera)
    {
        qDebug() << "OSGCamera::detach" << camera;

        if (camera != this->camera) {
            qWarning() << "OSGCamera::detach - camera not attached" << camera;
            return;
        }
        this->camera = NULL;

#ifdef USE_OSGEARTH
        if (logDepthBuffer) {
            logDepthBuffer->uninstall(camera);
            delete logDepthBuffer;
            logDepthBuffer = NULL;
        }
#endif
    }

    void attachManipulator(osgViewer::View *view)
    {
        qDebug() << "OSGCamera::attachManipulator" << view;

        osgGA::CameraManipulator *cm = NULL;

        switch (manipulatorMode) {
        case ManipulatorMode::Default:
        {
            qDebug() << "OSGCamera::attachManipulator - use TrackballManipulator";
            osgGA::TrackballManipulator *tm = new osgGA::TrackballManipulator();
            // Set the minimum distance of the eye point from the center before the center is pushed forward.
            // tm->setMinimumDistance(1, true);
            cm = tm;
            break;
        }
        case ManipulatorMode::User:
            qDebug() << "OSGCamera::attachManipulator - no camera manipulator";
            // disable any installed camera manipulator
            // TODO create and use own camera manipulator to avoid disabling ON_DEMAND frame update scheme
            // see https://github.com/gwaldron/osgearth/commit/796daf4792ccaf18ae7eb6a5cb268eef0d42888d
            // TODO see StandardManipulator for an example on how to react to events (to have FOV changes without the need for an update callback?)
            cm = NULL;
            break;
        case ManipulatorMode::Earth:
        {
#ifdef USE_OSGEARTH
            qDebug() << "OSGCamera::attachManipulator - use EarthManipulator";
            osgEarth::Util::EarthManipulator *em = new osgEarth::Util::EarthManipulator();
            em->getSettings()->setThrowingEnabled(true);
            cm = em;
#endif
            break;
        }
        case ManipulatorMode::Track:
            qDebug() << "OSGCamera::attachManipulator - use NodeTrackerManipulator";
            if (trackNode && trackNode->node()) {
                // setup tracking camera
                // TODO when camera is thrown, then changing attitude has jitter
                osgGA::NodeTrackerManipulator *ntm = new osgGA::NodeTrackerManipulator(
                    /*osgGA::StandardManipulator::COMPUTE_HOME_USING_BBOX | osgGA::StandardManipulator::DEFAULT_SETTINGS*/);
                switch (trackerMode) {
                case TrackerMode::NodeCenter:
                    ntm->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
                    break;
                case TrackerMode::NodeCenterAndAzim:
                    ntm->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_AZIM);
                    break;
                case TrackerMode::NodeCenterAndRotation:
                    ntm->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
                    break;
                }
                ntm->setTrackNode(trackNode->node());
                ntm->setVerticalAxisFixed(false);
                cm = ntm;
            } else {
                qWarning() << "OSGCamera::attachManipulator - no track node provided.";
                cm = NULL;
            }
            break;
        default:
            qWarning() << "OSGCamera::attachManipulator - should not reach here!";
            break;
        }

        view->setCameraManipulator(cm, false);
        if (cm && sceneNode && sceneNode->node()) {
            qDebug() << "OSGCamera::attachManipulator - camera node" << sceneNode;
            // set node used to auto compute home position
            // needs to be done after setting the manipulator on the view as the view will set its scene as the node
            cm->setNode(sceneNode->node());
        }
        if (cm) {
            view->home();
        }
    }

    void detachManipulator(osgViewer::View *view)
    {
        qDebug() << "OSGCamera::detachManipulator" << view;

        view->setCameraManipulator(NULL, false);
    }

    void updateFieldOfView()
    {
        qDebug() << "OSGCamera::updateCameraFOV" << fieldOfView;

        double fovy, ar, zn, zf;
        camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
        fovy = fieldOfView;
        camera->setProjectionMatrixAsPerspective(fovy, ar, zn, zf);
    }

    void updateAspectRatio()
    {
        osg::Viewport *viewport = camera->getViewport();
        double aspectRatio = static_cast<double>(viewport->width()) / static_cast<double>(viewport->height());

        qDebug() << "OSGCamera::updateAspectRatio" << aspectRatio;

        double fovy, ar, zn, zf;
        camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
        ar = aspectRatio;
        camera->setProjectionMatrixAsPerspective(fovy, ar, zn, zf);
    }

    void updatePosition()
    {
        if (manipulatorMode != ManipulatorMode::User) {
            return;
        }
        // Altitude mode is absolute (absolute height above MSL/HAE)
        // HAE : Height above ellipsoid. This is the default.
        // MSL : Height above Mean Sea Level (MSL) if a geoid separation value is specified.
        // TODO handle the case where the terrain SRS is not "wgs84"
        // TODO check if position is not below terrain?
        // TODO compensate antenna height when source of position is GPS (i.e. subtract antenna height from altitude) ;)

        // Camera position
        osg::Matrix cameraPosition;

#ifdef USE_OSGEARTH
        osgEarth::GeoPoint geoPoint = osgQtQuick::toGeoPoint(position);
        if (clampToTerrain) {
            if (sceneNode) {
                osgEarth::MapNode *mapNode = osgEarth::MapNode::findMapNode(sceneNode->node());
                if (mapNode) {
                    intoTerrain = clampGeoPoint(geoPoint, 0.5f, mapNode);
                } else {
                    qWarning() << "OSGCamera::updateNode - scene data does not contain a map node";
                }
            }
        }

        geoPoint.createLocalToWorld(cameraPosition);
#endif

        // Camera orientation
        // By default the camera looks toward -Z, we must rotate it so it looks toward Y
        osg::Matrix cameraRotation;
        cameraRotation.makeRotate(osg::DegreesToRadians(90.0), osg::Vec3(1.0, 0.0, 0.0),
                                  osg::DegreesToRadians(0.0), osg::Vec3(0.0, 1.0, 0.0),
                                  osg::DegreesToRadians(0.0), osg::Vec3(0.0, 0.0, 1.0));

        // Final camera matrix
        double roll  = osg::DegreesToRadians(attitude.x());
        double pitch = osg::DegreesToRadians(attitude.y());
        double yaw   = osg::DegreesToRadians(attitude.z());
        osg::Matrix cameraMatrix = cameraRotation
                                   * osg::Matrix::rotate(roll, osg::Vec3(0, 1, 0))
                                   * osg::Matrix::rotate(pitch, osg::Vec3(1, 0, 0))
                                   * osg::Matrix::rotate(yaw, osg::Vec3(0, 0, -1)) * cameraPosition;

        // Inverse the camera's position and orientation matrix to obtain the view matrix
        cameraMatrix = osg::Matrix::inverse(cameraMatrix);
        camera->setViewMatrix(cameraMatrix);
    }

    OSGCamera *const self;

    osg::ref_ptr<osg::Camera> camera;

    // Camera vertical field of view in degrees
    qreal   fieldOfView;

    OSGNode *sceneNode;

    ManipulatorMode::Enum manipulatorMode;

    // for NodeTrackerManipulator
    TrackerMode::Enum     trackerMode;
    OSGNode *trackNode;

    bool    logDepthBufferEnabled;
#ifdef USE_OSGEARTH
    osgEarth::Util::LogarithmicDepthBuffer *logDepthBuffer;
#endif

    bool clampToTerrain;
    bool intoTerrain;

    QVector3D attitude;
    QVector3D position;

private slots:
    void onSceneNodeChanged(osg::Node *node)
    {
        qDebug() << "OSGCamera::onSceneNodeChanged" << node;
        qWarning() << "OSGCamera::onSceneNodeChanged - needs to be implemented";
    }

    void onTrackNodeChanged(osg::Node *node)
    {
        qDebug() << "OSGCamera::onTrackNodeChanged" << node;
        qWarning() << "OSGCamera::onTrackNodeChanged - needs to be implemented";
    }
};

/* class OSGCamera */

enum DirtyFlag { FieldOfView = 1 << 0, Position = 1 << 1, Attitude = 1 << 2 };

OSGCamera::OSGCamera(QObject *parent) : OSGNode(parent), h(new Hidden(this))
{
    qDebug() << "OSGCamera::OSGCamera";
}

OSGCamera::~OSGCamera()
{
    qDebug() << "OSGCamera::~OSGCamera";
    delete h;
}

qreal OSGCamera::fieldOfView() const
{
    return h->fieldOfView;
}

void OSGCamera::setFieldOfView(qreal arg)
{
    if (h->fieldOfView != arg) {
        h->fieldOfView = arg;
        setDirty(FieldOfView);
        emit fieldOfViewChanged(fieldOfView());
    }
}

OSGNode *OSGCamera::sceneNode()
{
    return h->sceneNode;
}

void OSGCamera::setSceneNode(OSGNode *node)
{
    if (h->acceptSceneNode(node)) {
        emit sceneNodeChanged(node);
    }
}

ManipulatorMode::Enum OSGCamera::manipulatorMode() const
{
    return h->manipulatorMode;
}

void OSGCamera::setManipulatorMode(ManipulatorMode::Enum mode)
{
    if (h->acceptManipulatorMode(mode)) {
        emit manipulatorModeChanged(manipulatorMode());
    }
}

OSGNode *OSGCamera::trackNode() const
{
    return h->trackNode;
}

void OSGCamera::setTrackNode(OSGNode *node)
{
    if (h->acceptTrackNode(node)) {
        emit trackNodeChanged(node);
    }
}

TrackerMode::Enum OSGCamera::trackerMode() const
{
    return h->trackerMode;
}

void OSGCamera::setTrackerMode(TrackerMode::Enum mode)
{
    if (h->trackerMode != mode) {
        h->trackerMode = mode;
        emit trackerModeChanged(trackerMode());
    }
}

bool OSGCamera::clampToTerrain() const
{
    return h->clampToTerrain;
}

void OSGCamera::setClampToTerrain(bool arg)
{
    if (h->clampToTerrain != arg) {
        h->clampToTerrain = arg;
        emit clampToTerrainChanged(clampToTerrain());
    }
}

bool OSGCamera::intoTerrain() const
{
    return h->intoTerrain;
}

QVector3D OSGCamera::attitude() const
{
    return h->attitude;
}

void OSGCamera::setAttitude(QVector3D arg)
{
    if (h->attitude != arg) {
        h->attitude = arg;
        setDirty(Attitude);
        emit attitudeChanged(attitude());
    }
}

QVector3D OSGCamera::position() const
{
    return h->position;
}

void OSGCamera::setPosition(QVector3D arg)
{
    if (h->position != arg) {
        h->position = arg;
        setDirty(Position);
        emit positionChanged(position());
    }
}

bool OSGCamera::logarithmicDepthBuffer()
{
    return h->logDepthBufferEnabled;
}

void OSGCamera::setLogarithmicDepthBuffer(bool enabled)
{
    if (h->logDepthBufferEnabled != enabled) {
        h->logDepthBufferEnabled = enabled;
        emit logarithmicDepthBufferChanged(logarithmicDepthBuffer());
    }
}

void OSGCamera::update()
{
    if (isDirty(FieldOfView)) {
        h->updateFieldOfView();
    }
    if (isDirty(Position | Attitude)) {
        h->updatePosition();
    }
}

void OSGCamera::attach(osgViewer::View *view)
{
    h->attachCamera(view->getCamera());
    h->attachManipulator(view);
    update();
    clearDirty();
}

void OSGCamera::detach(osgViewer::View *view)
{
    h->detachManipulator(view);
    h->detachCamera(view->getCamera());
}
} // namespace osgQtQuick

#include "OSGCamera.moc"
