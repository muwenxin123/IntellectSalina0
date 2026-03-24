#ifndef AGRIMODEL_H_
#define AGRIMODEL_H_
 
#include <QString>
#include <QVariant>
#include <memory>

#include <osg/Node>
#include <osgGA/GUIEventHandler>

#include "QCString.h"

class AgriModelParam
{
public:
	AgriModelParam()
		:modelName("")
		, modelPath("")
		, lon(0.0)
		, lat(0.0)
		, height(0.0)
	{};

	std::string				modelName;
	std::string				modelPath;
	double					lon;
	double					lat;
	double					height;
};

typedef	std::shared_ptr<AgriModelParam>			AgriModelParamPtr;
typedef std::vector<AgriModelParamPtr>			AgriModelParamPtrVec;

Q_DECLARE_METATYPE(AgriModelParam)

class AgriModelUtil
{
public:
	static osg::ref_ptr<osg::Node> createModelNode(AgriModelParamPtr modelParam);
};

class AgriNodePickHandler :public osgGA::GUIEventHandler
{
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};

/*
	// test
	AgriModelParamPtr modelParam = std::make_shared<AgriModelParam>();
	osg::ref_ptr<osg::Node> modelNode = AgriModelUtil::createModelNode(modelParam);

	if (modelNode.valid())
	{
		osg::ref_ptr<osg::Group> groupSensor = new osg::Group;
		groupSensor->setName("groupSensor");
		groupSensor->addChild(modelNode);

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		mapNode->addChild(groupSensor.get());

		if (osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer)))
		{
			viewer->addEventHandler(new nodePick);
		}
	}
*/
#endif  // AgriModel_H