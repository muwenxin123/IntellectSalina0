#ifndef DETECTION_DATA_H
#define DETECTION_DATA_H

#include <vector>
#include <array>
#include <string>
#include <QImage>
#include <nlohmann/json.hpp>
#include <QTime>
#include <QPainter>
#include <memory>

namespace WS {
	// 基础类型定义
	using Color = std::array<uint8_t, 4>;        // [R, G, B, A] 0~255
	using Point = std::array<short, 2>;          // [x, y] 相对坐标
	using Box = std::array<short, 5>;            // [x, y, width, height, radian] 相对坐标，弧度逆时针

												 // 标签
	struct Label {
		Color color = { 255, 255, 255, 127 };      // 标签背景颜色
		std::string name;                         // 标签名称
		Point position = { 0, 0 };                 // 标签位置

		Label() = default;
	};

	// Bounding Box
	struct BBox {
		bool showDirection = false;              // 是否显示方向
		float score = 0.0f;                       // 置信度
		Box bbox = { 0, 0, 0, 0, 0 };              // 包围框
		Color color = { 0, 0, 0, 255 };            // 边界框颜色
		std::string label;                        // 边界框标签
		std::string groupId;                      // 所属 group 的 id
		int trackId;
		BBox() = default;
	};


	// 多边形或 ROI
	struct Polygon {
		bool showDirection = false;              // 是否显示方向
		bool fill = false;                        // 是否填充颜色
		int fill_rule = 0;                        // 0: 外侧, 1: 内侧
		Color color = { 0, 0, 0, 255 };            // 多边形颜色
		std::string groupId;                      // 所属 group 的 id
		std::string label;                        // 多边形标签
		std::vector<Point> points;                // 多边形点集

		Polygon() = default;
	};

	// 线条
	struct Line {
		std::string label;                        // 线条标签
		bool showDirection = false;              // 是否显示方向
		bool showPoint = false;                  // 是否显示折点
		std::vector<Point> points;                // 线条坐标点集合
		Color color = { 0, 0, 0, 255 };            // 线条颜色
		std::string groupId;                      // 所属 group 的 id

		Line() = default;
	};

	// 圆形
	struct Circle {
		std::string label;                        // 圆标签
		Point position = { 0, 0 };                 // 圆心坐标
		float radius = 0.0f;                      // 圆半径（相对值）
		Color color = { 0, 0, 0, 255 };            // 圆的颜色
		bool fill = false;                        // 是否填充
		std::string groupId;                      // 所属 group 的 id

		Circle() = default;
	};

	// 遮罩
	struct Mask {
		std::string label;                        // 遮罩标签
		std::string url;                          // 遮罩图片的 URL
		Point position = { 0, 0 };                 // 遮罩位置
		Color color = { 0, 0, 0, 255 };            // 遮罩颜色
		std::string groupId;                      // 所属 group 的 id

		Mask() = default;
	};

	// 渲染元数据
	struct RenderMeta {
		std::string url;                          // 原始内容的 URL
		std::string rendered_url;                 // 渲染后内容的 URL
		std::string type;                         // 'video' | 'image'
		int height = 0;                           // 图片高度
		int width = 0;                            // 图片宽度
		std::string thumbnail_url;                // 缩略图的 URL
		int thumbnail_height = 0;                 // 缩略图高度
		int thumbnail_width = 0;                  // 缩略图宽度
		std::string videoid;					  //视频流编号
		RenderMeta() = default;
	};

	// 完整的检测结果结构体
	struct DetectionData {
		RenderMeta metadata;                      // 渲染元数据
		std::vector<Label> labels;
		std::vector<BBox> bboxes;
		std::vector<Polygon> polygons;
		std::vector<Line> lines;
		std::vector<Circle> circles;
		std::vector<Mask> masks;

		QImage image;                             // 图像数据


		DetectionData() = default;
	};

	// JSON解析函数
	class DetectionParser {
	public:
		static bool DetectionParser::parseFromJson(const nlohmann::json& j, DetectionData &data);

	private:
		static Color parseColor(const nlohmann::json& j);
		static Point parsePoint(const nlohmann::json& j);
		static Box parseBox(const nlohmann::json& j);
		static Label parseLabel(const nlohmann::json& j);
		static BBox parseBBox(const nlohmann::json& j);
		static Polygon parsePolygon(const nlohmann::json& j);
		static Line parseLine(const nlohmann::json& j);
		static Circle parseCircle(const nlohmann::json& j);
		static Mask parseMask(const nlohmann::json& j);
		static RenderMeta parseRenderMeta(const nlohmann::json& j);
	};

	// 管理类
	class DetectionManager {
	private:
		DetectionManager() = default;

	public:
		static DetectionManager& instance() {
			static DetectionManager manager;
			return manager;
		}

		void updateData(const DetectionData& data) {
			m_currentData = data;
		}

		DetectionData getData() const {
			return m_currentData;
		}

		void clearData() {
			m_currentData = DetectionData();
		}

		bool hasData() const {
			return !m_currentData.bboxes.empty() ||
				!m_currentData.polygons.empty() ||
				!m_currentData.lines.empty() ||
				!m_currentData.circles.empty() ||
				!m_currentData.image.isNull();
		}

	private:
		DetectionData m_currentData;
	};

	/*struct EventData
	{
		QImage image;
		QTime time;
		QString eventcontent;
	    int type;
	};*/
	// 在 wsProtocol.h 中添加或修改 EventData 结构

 struct EventData
	{
		QImage image;
		QTime time;
		QString eventcontent;
		int type;
		QPixmap recimage;
		std::string videoid;
		std::vector<BBox> bboxes;
		std::shared_ptr<QImage> imagePtr;
		int totalBoxesInImage;

		//// 新增字段：存储检测结果的详细信息
		struct DetectionSummary {
			int bboxCount = 0;           // 边界框数量
			int polygonCount = 0;        // 多边形数量
			int lineCount = 0;           // 线条数量
			int circleCount = 0;         // 圆形数量
			int maskCount = 0;           // 遮罩数量
			QStringList labels;          // 检测到的标签列表
			QStringList groupIds;        // 分组ID列表
		} detectionSummary;

		// 构造函数
		EventData() = default;

		// 从 DetectionData 创建 EventData 的构造函数
		EventData(const DetectionData& detectionData) {
			image = detectionData.image;
			time = QTime::currentTime();
			type = 1; // 检测事件类型

					  // 生成检测摘要
			detectionSummary.bboxCount = detectionData.bboxes.size();
			detectionSummary.polygonCount = detectionData.polygons.size();
			detectionSummary.lineCount = detectionData.lines.size();
			detectionSummary.circleCount = detectionData.circles.size();
			detectionSummary.maskCount = detectionData.masks.size();

			// 收集所有标签
			for (const auto& bbox : detectionData.bboxes) {
				if (!bbox.label.empty()) {
					detectionSummary.labels.append(QString::fromStdString(bbox.label));
				}
				if (!bbox.groupId.empty()) {
					detectionSummary.groupIds.append(QString::fromStdString(bbox.groupId));
				}
			}

			for (const auto& polygon : detectionData.polygons) {
				if (!polygon.label.empty()) {
					detectionSummary.labels.append(QString::fromStdString(polygon.label));
				}
			}

			// 移除重复的标签和分组ID
			detectionSummary.labels.removeDuplicates();
			detectionSummary.groupIds.removeDuplicates();

			// 生成事件内容
			eventcontent = generateEventContentFromDetection();
		}

	private:
		QString generateEventContentFromDetection() {
			QStringList events;

			if (detectionSummary.bboxCount > 0) {
				events.append(QString("边界框:%1").arg(detectionSummary.bboxCount));
			}
			if (detectionSummary.polygonCount > 0) {
				events.append(QString("多边形:%1").arg(detectionSummary.polygonCount));
			}
			if (detectionSummary.lineCount > 0) {
				events.append(QString("线条:%1").arg(detectionSummary.lineCount));
			}
			if (detectionSummary.circleCount > 0) {
				events.append(QString("圆形:%1").arg(detectionSummary.circleCount));
			}
			if (detectionSummary.maskCount > 0) {
				events.append(QString("遮罩:%1").arg(detectionSummary.maskCount));
			}

			// 如果有具体的标签信息，添加到事件内容中
			if (!detectionSummary.labels.isEmpty()) {
				events.append(QString("标签:%1").arg(detectionSummary.labels.join(",")));
			}

			return events.join("; ");
		}
	};
}


#endif // DETECTION_DATA_H