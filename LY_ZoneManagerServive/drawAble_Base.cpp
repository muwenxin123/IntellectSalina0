#include "drawAble_Base.h"

drawAble_Base::drawAble_Base()
{
}

drawAble_Base::drawAble_Base(osgEarth::MapNode * _mapNode)
{
	mapNode = _mapNode;
}

drawAble_Base::~drawAble_Base()
{
}

void drawAble_Base::setGroup(osg::Group * group)
{
	tempGroup = group;
}

void drawAble_Base::setMapNode(osgEarth::MapNode * _mapNode)
{
	mapNode = _mapNode;
}

void drawAble_Base::setColor(unsigned int _color, float _color_A)
{
	color = _color;
	color_A = _color_A;
}

void drawAble_Base::setRectInfo(double _width, double _heitht, float _angle)
{
	width = _width;
	height = _heitht;
	angle = _angle;
}

void drawAble_Base::setID(std::string _featureNodeID)
{
	featureNodeID = _featureNodeID;
}

std::string drawAble_Base::getID()
{
	return featureNodeID;
}

std::vector<osg::Vec3d> drawAble_Base::getPoints()
{
	return points;
}

osgEarth::Annotation::AnnotationNode * drawAble_Base::getFeatureNode()
{
	return featureNode;
}

