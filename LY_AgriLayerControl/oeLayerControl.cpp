#include "oeLayerControl.h"

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/ImageLayer>
#include <osgEarth/ElevationLayer>

oeLayerControl::oeLayerControl()
{
}

oeLayerControl::oeLayerControl(osgEarth::MapNode *_mapNode)
{
    mapNode = _mapNode;
}

oeLayerControl::~oeLayerControl()
{
}

void oeLayerControl::setMapNode(osgEarth::MapNode *_mapNode)
{
    mapNode = _mapNode;
}

bool oeLayerControl::resetLayerOrder(std::string LayerName, int orderIndex)
{
    if (mapNode == nullptr)
    {
        return false;
    }
    osgEarth::Map *map = mapNode->getMap();
    if (map == nullptr)
    {
        return false;
    }
    if (map->getImageLayerByName(LayerName))
    {
        osgEarth::ImageLayer *layer = map->getImageLayerByName(LayerName);
        if (map->getImageLayerAt(orderIndex) == layer)
        {
            return true;
        }
        else
        {
            map->moveImageLayer(layer, orderIndex);
            if (loadLayerFromMpaNode())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    if (map->getElevationLayerByName(LayerName))
    {
        osgEarth::ElevationLayer *layer = map->getElevationLayerByName(LayerName);
        if (map->getElevationLayerAt(orderIndex) == layer)
        {
            return true;
        }
        else
        {
            map->moveElevationLayer(layer, orderIndex);
            if (loadLayerFromMpaNode())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
	return false;
}

bool oeLayerControl::setLayerShowOrDis(std::string LayerName, bool isShow)
{
    if (mapNode == nullptr)
    {
        return false;
    }
    osgEarth::Map *map = mapNode->getMap();
    if (map == nullptr)
    {
        return false;
    }
    if (map->getImageLayerByName(LayerName))
    {
        map->getImageLayerByName(LayerName)->setVisible(isShow);
        if (loadLayerFromMpaNode())
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    if (osgEarth::ElevationLayer * eleLayer = map->getElevationLayerByName(LayerName))
    {

#if 0
		eleLayer->setVisible(isShow); // ²âÊÔÎ´ÉúÐ§20250529
#else
		osgEarth::ElevationLayerVector elevationLayers;
		map->getElevationLayers(elevationLayers);
		for (int eleIndex = 0; eleIndex < elevationLayers.size(); eleIndex++)
		{
			osgEarth::ElevationLayer * pEleLayer = elevationLayers.at(eleIndex).get();
			if (eleLayer->getName() == LayerName)
			{
				pEleLayer->setVisible(isShow);
			}
		}
#endif
        if (loadLayerFromMpaNode())
        {
            return true;
        }
		else
		{
			return false;
		}
    }
    if (map->getModelLayerByName(LayerName))
    {
        map->getModelLayerByName(LayerName)->setVisible(isShow);
        if (loadLayerFromMpaNode())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
	return false;
}

std::vector<std::pair<std::string, bool>> oeLayerControl::getImageLayerOrder()
{
    return imageLayerNames;
}

std::vector<std::pair<std::string, bool>> oeLayerControl::getElevationLayerOrder()
{
    return elevationLayerNames;
}

std::vector<std::pair<std::string, bool>> oeLayerControl::getModelLyaerOrder()
{
    return modelLayerNames;
}

bool oeLayerControl::loadLayerFromMpaNode()
{
    elevationLayerNames.clear();
    imageLayerNames.clear();
    bool isSecc = false;
    if (mapNode == nullptr)
    {
        return isSecc;
    }

    osgEarth::Map *map = mapNode->getMap();
    if (map == NULL)
    {
        return isSecc;
    }
    osgEarth::ImageLayerVector imageLayers;
    osgEarth::ElevationLayerVector elevationLayers;
    osgEarth::ModelLayerVector modelLayers;
    map->getImageLayers(imageLayers);
    map->getElevationLayers(elevationLayers);
    map->getModelLayers(modelLayers);

    for (int imageIndex = 0; imageIndex < imageLayers.size(); imageIndex++)
    {
        osgEarth::ImageLayer *imageLayer = imageLayers.at(imageIndex).get();
        imageLayerNames.push_back(std::make_pair(imageLayer->getName(), imageLayer->getVisible()));
    }
    for (int eleIndex = 0; eleIndex < elevationLayers.size(); eleIndex++)
    {
        osgEarth::ElevationLayer *eleLayer = elevationLayers.at(eleIndex).get();
        elevationLayerNames.push_back(std::make_pair(eleLayer->getName(), eleLayer->getVisible()));
    }
    for (int modelIndex = 0; modelIndex < modelLayers.size(); modelIndex++)
    {
        osgEarth::ModelLayer *modelLayer = modelLayers.at(modelIndex).get();
        modelLayerNames.push_back(std::make_pair(modelLayer->getName(), modelLayer->getVisible()));
    }
    return !isSecc;
}
