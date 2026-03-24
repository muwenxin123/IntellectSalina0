#include "CoordinateTransform.h"

osg::Vec3d CoordinateTransform::screen2World(const osgViewer::View *viewer, osg::Vec3 screenPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	osg::Matrix invertVPW;
	invertVPW.invert(mVPW);
	vec3 = screenPoint * invertVPW;
	return vec3;
}

osg::Vec3d CoordinateTransform::world2Screen(const osgViewer::View *viewer, osg::Vec3 worldPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	osg::Matrix mVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	vec3 = worldPoint * mVPW;
	return vec3;
}

osg::Vec3d CoordinateTransform::world2Camera(const osgViewer::View *viewer, osg::Vec3 worldPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	osg::Matrix mV = camera->getViewMatrix();
	vec3 = worldPoint * mV;
	return vec3;
}

osg::Vec3d CoordinateTransform::camera2World(const osgViewer::View *viewer, osg::Vec3 cameraPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mV = camera->getViewMatrix();
	osg::Matrix invertmV;
	invertmV.invert(mV);
	vec3 = cameraPoint * invertmV;
	return vec3;
}

osg::Vec3d CoordinateTransform::screen2Camera(const osgViewer::View *viewer, osg::Vec3 screenPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	osg::Matrix mPW = camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	osg::Matrix invertmPW;
	invertmPW.invert(mPW);
	vec3 = screenPoint * invertmPW;
	return vec3;
}

osg::Vec3d CoordinateTransform::camera2Screen(const osgViewer::View *viewer, osg::Vec3 cameraPoint)
{
	osg::Vec3d vec3;
	auto camera = viewer->getCamera();
	//osg::Vec3d vScreen(x,y, 0);
	osg::Matrix mPW = camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
	vec3 = cameraPoint * mPW;
	return vec3;
}
