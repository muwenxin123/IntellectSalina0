/*
三维地球已有图层管理：
软件中读取 .earth文件，加载影像、高程等
该类从传入的osgEarth：：MapNode读入软件中加载的图层，
(暂时只加载影像图层imageLayer和高程图层elevationLayer）

作用： 可调整各图层顺序，显隐某一图层

*/
#pragma once

#include <vector>
#include <string>


namespace osgEarth
{
class MapNode;
}

class oeLayerControl
{
public:
    oeLayerControl();
    oeLayerControl(osgEarth::MapNode *_mapNode);
    ~oeLayerControl();

    void setMapNode(osgEarth::MapNode *_mapNode);

    bool resetLayerOrder(std::string LayerName, int orderIndex);
    bool setLayerShowOrDis(std::string LayerName, bool isShow);

    std::vector<std::pair<std::string, bool>> getImageLayerOrder();
    std::vector<std::pair<std::string, bool>> getElevationLayerOrder();
    std::vector<std::pair<std::string, bool>> getModelLyaerOrder();

public:
    //从mapNode中读取图层
    bool loadLayerFromMpaNode();

private:
    osgEarth::MapNode *mapNode = nullptr;

    std::vector<std::pair<std::string, bool>>		imageLayerNames;			//影像图层顺序  <名称， 显隐>
    std::vector<std::pair<std::string, bool>>		elevationLayerNames;		//高程图层顺序  <名称， 显隐>
    std::vector<std::pair<std::string, bool>>		modelLayerNames;			//模型图层顺序  <名称， 显隐>
};