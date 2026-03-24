#pragma once

#include <vector>
#include <string>

#include <osg/Vec3d>

namespace osg
{
	class Node;
	class EllipsoidModel;
	class Group;
}
namespace osgEarth
{
	class MapNode;
	class SpatialReference;
	namespace Annotation
	{
		class AnnotationNode;
		class FeatureNode;
		class FeatureEditor;
	}
	namespace Features
	{
		class Feature;
	}

	namespace Symbology
	{
		class Style;
		class Geometry;
	}
}

class drawAble_Base
{
public:
	drawAble_Base();
	drawAble_Base(osgEarth::MapNode* _mapNode);
	~drawAble_Base();

	virtual void OnEarthMessage(int vParam, void* lParam) = 0;

	void setGroup(osg::Group* group);
	void setMapNode(osgEarth::MapNode* _mapNode);
	void setColor(unsigned int _color, float _color_A);

	void setRectInfo(double _width, double _heitht, float _angle);

	void setID(std::string _featureNodeID);
	std::string getID();

	std::vector<osg::Vec3d> getPoints();

	osgEarth::Annotation::AnnotationNode* getFeatureNode();

protected:

	std::string   featureNodeID;

	osgEarth::MapNode* mapNode = nullptr;

	osg::Group* tempGroup = nullptr;
	osgEarth::Features::Feature* feature = nullptr;
	osgEarth::Annotation::AnnotationNode* featureNode = nullptr;
	osgEarth::Symbology::Style* geomStyle = nullptr;

	std::vector<osg::Vec3d> points;

	unsigned int color = 255;
	float color_A = 0.5;

	bool   isTrackMoving = true;

	double width;
	double height;
	float angle;
};

