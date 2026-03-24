#include "drawAble_LineString.h"

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

drawAble_LineString::drawAble_LineString()
{
}

drawAble_LineString::~drawAble_LineString()
{
}

void drawAble_LineString::OnEarthMessage(int wParam, void * lParam)
{
	pos3d *p3d = (pos3d *)lParam;
	osg::Vec3d pos(p3d->longitude, p3d->latitude, p3d->altitude);

	switch (wParam)
	{
	case IEarthEventHandler::Earth_LDOUBLECLICK:
	{
		isTrackMoving = true;
		points.push_back(pos);
		if (points.size() >= 2)
		{
			{
				feature->getGeometry()->pop_back();
				feature->getGeometry()->push_back(pos);
				feature->getGeometry()->push_back(pos);
				dynamic_cast<osgEarth::Annotation::FeatureNode*>(featureNode)->dirty();
			}
		}
	}
	break;
	case IEarthEventHandler::Earth_RDOUBLECLICK:
	{
		if (points.empty())
			return;
		feature->getGeometry()->pop_back();
		dynamic_cast<osgEarth::Annotation::FeatureNode*>(featureNode)->dirty();
		isTrackMoving = false;
	}
	break;
	case IEarthEventHandler::Earth_MOUSE_MOVE:
	{
		if (isTrackMoving == false)
			return;
		if (points.size() == 1)
		{

		}

		if (points.size() >= 1)
		{

			if (feature == nullptr)
			{
				osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_LINESTRING, &points);

				geoMery->push_back(pos);

				feature = new osgEarth::Features::Feature(geoMery, mapNode->getMapSRS()->getGeographicSRS());

				geomStyle = new osgEarth::Style;

				osgEarth::Symbology::Color tempCo(GETR(color) / 255.0, GETG(color) / 255.0, GETB(color) / 255.0, color_A);

				geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
				geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
				geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;
				geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->fill().mutable_value().color() = tempCo;

				geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

				geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
					->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
				geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;
				geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
					->verticalOffset() = 0.1;

				featureNode = new osgEarth::Annotation::FeatureNode(mapNode, feature, *geomStyle);

				tempGroup->addChild(featureNode);
			}
			else
			{
				feature->getGeometry()->pop_back();
				feature->getGeometry()->push_back(pos);
				dynamic_cast<osgEarth::Annotation::FeatureNode*>(featureNode)->dirty();
			}
		}

	}
	break;
	default:
		break;
	}
}
