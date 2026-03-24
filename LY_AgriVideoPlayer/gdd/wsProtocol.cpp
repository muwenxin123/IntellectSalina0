#include "wsProtocol.h"
#include <iostream>
#include <iostream>
#include <cmath>

namespace WS {

bool DetectionParser::parseFromJson(const nlohmann::json& j, DetectionData &data)
{   
    try {
        // 解析渲染元数据
        if (j.contains("metadata")) {
            data.metadata = parseRenderMeta(j["metadata"]);
        }
        
		// 解析 labels
		if (j.contains("labels") && j["labels"].is_array()) {
			for (const auto& label_json : j["labels"]) {
				data.labels.push_back(parseLabel(label_json));
			}
		}

		// 解析 bboxes
		if (j.contains("bboxes") && j["bboxes"].is_array()) {
			for (const auto& bbox_json : j["bboxes"]) {
				data.bboxes.push_back(parseBBox(bbox_json));
			}
		}

		// 解析 polygons
		if (j.contains("polygons") && j["polygons"].is_array()) {
			for (const auto& polygon_json : j["polygons"]) {
				data.polygons.push_back(parsePolygon(polygon_json));
			}
		}

		// 解析 lines
		if (j.contains("lines") && j["lines"].is_array()) {
			for (const auto& line_json : j["lines"]) {
				data.lines.push_back(parseLine(line_json));
			}
		}

		// 解析 circles
		if (j.contains("circles") && j["circles"].is_array()) {
			for (const auto& circle_json : j["circles"]) {
				data.circles.push_back(parseCircle(circle_json));
			}
		}

		// 解析 masks
		if (j.contains("masks") && j["masks"].is_array()) {
			for (const auto& mask_json : j["masks"]) {
				data.masks.push_back(parseMask(mask_json));
			}
		}


		return true;
    } catch (const std::exception& e) {
        std::cerr << "解析DetectionData错误: " << e.what() << std::endl;
    }
	return false;
}

Color DetectionParser::parseColor(const nlohmann::json& j)
{
    Color color = {0, 0, 0, 255};
    
    if (j.is_array() && j.size() >= 4) {
        for (size_t i = 0; i < 4 && i < j.size(); ++i) {
            color[i] = j[i];
        }
    }
    
    return color;
}

Point DetectionParser::parsePoint(const nlohmann::json& j)
{
    Point point = {0, 0};
    
    if (j.is_array() && j.size() >= 2) {
        for (size_t i = 0; i < 2 && i < j.size(); ++i) {
            point[i] = j[i];
        }
    }
    
    return point;
}

Box DetectionParser::parseBox(const nlohmann::json& j)
{
    Box box = {0, 0, 0, 0, 0};
    
    if (j.is_array() && j.size() >= 4)
	{
		int nS = std::max((size_t)5, j.size());
        for (size_t i = 0; i < nS; ++i) {
            box[i] = j[i];
        }
    }
    
    return box;
}

Label DetectionParser::parseLabel(const nlohmann::json& j)
{
    Label label;
    
    if (j.contains("color") && j["color"].is_array()) {
        label.color = parseColor(j["color"]);
    }
    
    if (j.contains("name") && j["name"].is_string()) {
        label.name = j["name"];
    }
    
    if (j.contains("position") && j["position"].is_array()) {
        label.position = parsePoint(j["position"]);
    }
    
    return label;
}

BBox DetectionParser::parseBBox(const nlohmann::json& j)
{
    BBox bbox;
    
    if (j.contains("showDirection") && j["showDirection"].is_boolean()) {
        bbox.showDirection = j["showDirection"];
    }
    
    if (j.contains("label") && j["label"].is_string()) {
        bbox.label = j["label"];
    }
    
    if (j.contains("score") && j["score"].is_number()) {
        bbox.score = j["score"];
    }
    
    if (j.contains("bbox") && j["bbox"].is_array()) {
        bbox.bbox = parseBox(j["bbox"]);
    }
    
    if (j.contains("color") && j["color"].is_array()) {
        bbox.color = parseColor(j["color"]);
    }
    
    if (j.contains("groupId") && j["groupId"].is_string()) {
        bbox.groupId = j["groupId"];
    }
    
	if (j.contains("track_id") && j["track_id"].is_number()) {
		bbox.trackId = j["track_id"];
	}

    return bbox;
}

Polygon DetectionParser::parsePolygon(const nlohmann::json& j)
{
    Polygon polygon;
    
    if (j.contains("label") && j["label"].is_string()) {
        polygon.label = j["label"];
    }
    
    if (j.contains("showDirection") && j["showDirection"].is_boolean()) {
        polygon.showDirection = j["showDirection"];
    }
    
    if (j.contains("points") && j["points"].is_array()) {
        for (const auto& point_json : j["points"]) {
            if (point_json.is_array()) {
                polygon.points.push_back(parsePoint(point_json));
            }
        }
    }
    
    if (j.contains("fill") && j["fill"].is_boolean()) {
        polygon.fill = j["fill"];
    }
    
    if (j.contains("fill_rule") && j["fill_rule"].is_number()) {
        polygon.fill_rule = j["fill_rule"];
    }
    
    if (j.contains("color") && j["color"].is_array()) {
        polygon.color = parseColor(j["color"]);
    }
    
    if (j.contains("groupId") && j["groupId"].is_string()) {
        polygon.groupId = j["groupId"];
    }
    
    return polygon;
}

Line DetectionParser::parseLine(const nlohmann::json& j)
{
    Line line;
    
    if (j.contains("label") && j["label"].is_string()) {
        line.label = j["label"];
    }
    
    if (j.contains("showDirection") && j["showDirection"].is_boolean()) {
        line.showDirection = j["showDirection"];
    }
    
    if (j.contains("showPoint") && j["showPoint"].is_boolean()) {
        line.showPoint = j["showPoint"];
    }
    
    if (j.contains("points") && j["points"].is_array()) {
        for (const auto& point_json : j["points"]) {
            if (point_json.is_array()) {
                line.points.push_back(parsePoint(point_json));
            }
        }
    }
    
    if (j.contains("color") && j["color"].is_array()) {
        line.color = parseColor(j["color"]);
    }
    
    if (j.contains("groupId") && j["groupId"].is_string()) {
        line.groupId = j["groupId"];
    }
    
    return line;
}

Circle DetectionParser::parseCircle(const nlohmann::json& j)
{
    Circle circle;
    
    if (j.contains("label") && j["label"].is_string()) {
        circle.label = j["label"];
    }
    
    if (j.contains("position") && j["position"].is_array()) {
        circle.position = parsePoint(j["position"]);
    }
    
    if (j.contains("radius") && j["radius"].is_number()) {
        circle.radius = j["radius"];
    }
    
    if (j.contains("color") && j["color"].is_array()) {
        circle.color = parseColor(j["color"]);
    }
    
    if (j.contains("fill") && j["fill"].is_boolean()) {
        circle.fill = j["fill"];
    }
    
    if (j.contains("groupId") && j["groupId"].is_string()) {
        circle.groupId = j["groupId"];
    }
    
    return circle;
}

Mask DetectionParser::parseMask(const nlohmann::json& j)
{
    Mask mask;
    
    if (j.contains("label") && j["label"].is_string()) {
        mask.label = j["label"];
    }
    
    if (j.contains("url") && j["url"].is_string()) {
        mask.url = j["url"];
    }
    
    if (j.contains("position") && j["position"].is_array()) {
        mask.position = parsePoint(j["position"]);
    }
    
    if (j.contains("color") && j["color"].is_array()) {
        mask.color = parseColor(j["color"]);
    }
    
    if (j.contains("groupId") && j["groupId"].is_string()) {
        mask.groupId = j["groupId"];
    }
    
    return mask;
}

RenderMeta DetectionParser::parseRenderMeta(const nlohmann::json& j)
{
    RenderMeta meta;
    
	if (j.contains("video") && j["video"].is_string()) {
		meta.videoid = j["video"];
	}

    if (j.contains("url") && j["url"].is_string()) {
        meta.url = j["url"];
    }
    
    if (j.contains("rendered_url") && j["rendered_url"].is_string()) {
        meta.rendered_url = j["rendered_url"];
    }
    
    if (j.contains("type") && j["type"].is_string()) {
        meta.type = j["type"];
    }
    
    if (j.contains("height") && j["height"].is_number()) {
        meta.height = j["height"];
    }
    
    if (j.contains("width") && j["width"].is_number()) {
        meta.width = j["width"];
    }
    
    if (j.contains("thumbnail_url") && j["thumbnail_url"].is_string()) {
        meta.thumbnail_url = j["thumbnail_url"];
    }
    
    if (j.contains("thumbnail_height") && j["thumbnail_height"].is_number()) {
        meta.thumbnail_height = j["thumbnail_height"];
    }
    
    if (j.contains("thumbnail_width") && j["thumbnail_width"].is_number()) {
        meta.thumbnail_width = j["thumbnail_width"];
    }
    

    return meta;
}

} // namespace ws