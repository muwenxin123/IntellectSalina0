#include "drawAble_Rect.h"

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>

#include <osgEarth/MapNode>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthAnnotation/FeatureNode>

#include <osgEarthAnnotation/FeatureEditing>

#include <osg/CoordinateSystemNode>

#include <osgEarthSymbology/PointSymbol>

#include <osg/Geometry>

#include "rgba.h"

#include <osgEarthAnnotation/RectangleNode>

drawAble_Rect::drawAble_Rect()
{
}

drawAble_Rect::~drawAble_Rect()
{

	osgEarth::Annotation::AnnotationNode;
	osgEarth::Annotation::CircleNode;
	osgEarth::Annotation::LocalizedNode;
	osgEarth::Annotation::PositionedAnnotationNode;
}

void drawAble_Rect::OnEarthMessage(int wParam, void * lParam)
{
	pos3d *p3d = (pos3d *)lParam;
	osg::Vec3d pos(p3d->longitude, p3d->latitude, p3d->altitude);

	switch (wParam)
	{
	case IEarthEventHandler::Earth_LDOUBLECLICK:
	{
		points.clear();
		points.push_back(pos);

		if (featureNode != nullptr)
		{
			tempGroup->removeChild(featureNode);
		}

		geomStyle = new osgEarth::Style;

		osgEarth::Symbology::Color tempCo(GETR(color) / 255.0, GETG(color) / 255.0, GETB(color) / 255.0, color_A);

		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()
			->fill()->color() = osgEarth::Symbology::Color(tempCo);
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->fill().mutable_value().color() = tempCo;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		featureNode = new osgEarth::Annotation::RectangleNode(mapNode,
			osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), pos, osgEarth::AltitudeMode::ALTMODE_RELATIVE),
			width, height, *geomStyle);
		tempGroup->addChild(featureNode);

		dynamic_cast<osgEarth::Annotation::RectangleNode*>(featureNode)->setLocalRotation(osg::Quat(osg::inDegrees(-angle), osg::Vec3d(0.0, 0.0, 1.0)));
	}
	break;
	case IEarthEventHandler::Earth_RDOUBLECLICK:
	{
		isTrackMoving = false;
	}
	break;
	case IEarthEventHandler::Earth_MOUSE_MOVE:
	{

	}
	break;
	default:
		break;
	}
}
