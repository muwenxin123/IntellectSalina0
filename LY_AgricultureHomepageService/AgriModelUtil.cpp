#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>
#include <osgEarth/SpatialReference>

#include <osgFX/Scribe>
#include <osgFX/Outline>

#include <map3d/IEarth.h>
#include <map3d/IGraphics.h>
#include <map3d/EarthEvent.h>

#include "AgriModelUtil.h"

osg::ref_ptr<osg::Node> AgriModelUtil::createModelNode(AgriModelParamPtr modelParam)
{
	if (!modelParam) return nullptr;

	osg::ref_ptr<osg::Node> pNode = osgDB::readNodeFile(modelParam->modelPath);
	if (!pNode)
	{
		osg::notify(osg::FATAL) << "模型加载失败: " << modelParam->modelPath << std::endl;
		return nullptr;
	}

	//pNode->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON); // 开启法线跟随缩放，避免模型缩放后变黑
	//pNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE); // 关闭模型光照

	osg::Matrix posMatrix;
	if (osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel)))
	{
		em->computeLocalToWorldTransformFromLatLongHeight(
			osg::inDegrees(modelParam->lat), osg::inDegrees(modelParam->lon), modelParam->height, posMatrix);
	}

	double scale = 1.0;
	osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
	osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(0.0), osg::Z_AXIS);
	osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	transform->setMatrix(finalMatrix);
	transform->addChild(pNode);
	transform->setName(modelParam->modelName);

	return transform;
}

bool AgriNodePickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::PUSH:
	{
		// 创建一个线段交集检测函数
		osgUtil::LineSegmentIntersector::Intersections intersections;
		osg::ref_ptr<osg::Node> node = new osg::Node();
		osg::ref_ptr<osg::Group> parent = new osg::Group();

		if (viewer->computeIntersections(ea.getX(), ea.getY(), intersections))
		{
			// 获取相交模型点的节点
			osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
			osg::NodePath& nodePath = intersection.nodePath;
			node = nodePath.back();

			// 判定所属的分组
			for (auto & path : nodePath)
			{
				if (path->getName() == "groupSensor")
				{
					// 点击节点切换高亮
					parent = dynamic_cast<osg::Group*> (nodePath[nodePath.size() - 2]); // 当前选择节点的父节点
					osgFX::Outline* ot = dynamic_cast<osgFX::Outline*>(parent.get());
					if (!ot)
					{
						osg::ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
						outline->setColor(osg::Vec4(1, 1, 0, 1));
						outline->setWidth(5);
						outline->addChild(node);
						parent->replaceChild(node, outline);
					}
					else
					{
						osg::Node::ParentList parentList = ot->getParents();
						osg::Node::ParentList::iterator itr = parentList.begin();
						(*itr)->replaceChild(ot, node);
					}
				}
			}
		}
		else
		{
			std::cout << "select nothing!" << std::endl;
		}
	}
	default:
		return false;
	}
}

