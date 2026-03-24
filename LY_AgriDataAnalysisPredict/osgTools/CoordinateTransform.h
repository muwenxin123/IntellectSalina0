#ifndef COORDINATETRANSFORM_H_
#define COORDINATETRANSFORM_H_
 
#include <osgViewer/Viewer>

namespace CoordinateTransform
{
	/* 屏幕转世界 */
	osg::Vec3d screen2World(const osgViewer::View *viewer, osg::Vec3 screenPoint);

	/* 世界到屏幕 */
	osg::Vec3d world2Screen(const osgViewer::View *viewer, osg::Vec3 worldPoint);

	/* 世界转相机 */
	osg::Vec3d world2Camera(const osgViewer::View *viewer, osg::Vec3 worldPoint);

	/* 相机转世界 */
	osg::Vec3d camera2World(const osgViewer::View *viewer, osg::Vec3 cameraPoint);

	/* 屏幕转相机 */
	osg::Vec3d screen2Camera(const osgViewer::View *viewer, osg::Vec3 screenPoint);

	/* 相机转屏幕 */
	osg::Vec3d camera2Screen(const osgViewer::View *viewer, osg::Vec3 cameraPoint);
}

#endif  // CoordinateTransform_H