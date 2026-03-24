#include "modelprocessor.h"
#include <QDebug>
#include <QFileInfo>
#include "PutTextZH.h"
#include <QFile>
#include "EventRecord.h"
#include "LY_AgriVideoPlayer.h"
#pragma execution_character_set("utf-8")
ModelProcessor::ModelProcessor(QObject *parent)
	: QObject(parent)
{
}

ModelProcessor::~ModelProcessor()
{
	QMutexLocker locker(&m_mutex);
	if (m_detector) {
		delete m_detector;
		m_detector = nullptr;
		qDebug() << "YOLO检测器已释放";
	}
}

bool ModelProcessor::loadModel(const QString &cfgPath, const QString &weightsPath)
{
	QMutexLocker locker(&m_mutex);

	if (cfgPath.isEmpty() || weightsPath.isEmpty()) {
		if (m_detector) {
			delete m_detector;
			m_detector = nullptr;
			m_currentCfgPath.clear();
			m_currentWeightsPath.clear();
		}
		return true;
	}

	// 检查文件是否存在
	if (!QFile::exists(cfgPath)) {
		qWarning() << "配置文件不存在:" << cfgPath;
		return false;
	}

	if (!QFile::exists(weightsPath)) {
		qWarning() << "权重文件不存在:" << weightsPath;
		return false;
	}

	// 如果已经是当前模型，直接返回
	if (cfgPath == m_currentCfgPath && weightsPath == m_currentWeightsPath) {
		return true;
	}

	// 释放旧模型
	if (m_detector) {
		delete m_detector;
		m_detector = nullptr;
	}

	try {
		// 创建新的检测器
		m_detector = new Detector(
			cfgPath.toStdString(),
			weightsPath.toStdString(),
			0
		);

		m_currentCfgPath = cfgPath;
		m_currentWeightsPath = weightsPath;

		qDebug() << "YOLO模型加载成功:"
			<< QFileInfo(cfgPath).fileName()
			<< "|"
			<< QFileInfo(weightsPath).fileName();

		return true;
	}
	catch (const std::exception &e) {
		qCritical() << "YOLO模型加载错误:" << e.what();
		return false;
	}
	catch (...) {
		qCritical() << "未知的YOLO模型加载错误";
		return false;
	}
}

bool ModelProcessor::isModelLoaded() const
{
	QMutexLocker locker(&m_mutex);
	return m_detector != nullptr;
}

cv::Mat ModelProcessor::qimageToMat(const QImage &img)
{
	switch (img.format()) {
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		return cv::Mat(img.height(), img.width(), CV_8UC4,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	case QImage::Format_RGB888:
		return cv::Mat(img.height(), img.width(), CV_8UC3,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	case QImage::Format_Grayscale8:
		return cv::Mat(img.height(), img.width(), CV_8UC1,
			const_cast<uchar*>(img.bits()),
			static_cast<size_t>(img.bytesPerLine()));
	default:
		QImage converted = img.convertToFormat(QImage::Format_RGB888);
		return cv::Mat(converted.height(), converted.width(), CV_8UC3,
			const_cast<uchar*>(converted.bits()),
			static_cast<size_t>(converted.bytesPerLine()));
	}
}

QImage ModelProcessor::matToQImage(const cv::Mat &mat)
{
	switch (mat.type()) {
	case CV_8UC1: // 8-bit, 1 channel
		return QImage(mat.data, mat.cols, mat.rows,
			static_cast<int>(mat.step), QImage::Format_Grayscale8);
	case CV_8UC3: // 8-bit, 3 channels
		return QImage(mat.data, mat.cols, mat.rows,
			static_cast<int>(mat.step), QImage::Format_RGB888).rgbSwapped();
	case CV_8UC4: // 8-bit, 4 channels
		return QImage(mat.data, mat.cols, mat.rows,
			static_cast<int>(mat.step), QImage::Format_ARGB32);
	default:
		qWarning() << "不支持的图像格式:" << mat.type();
		return QImage();
	}
}

void ModelProcessor::drawDetections(cv::Mat &frame, const std::vector<bbox_t> &detections, const VideoInfoData videoInfo)
{
	for (auto iter = detections.begin();
		iter != detections.end(); ++iter) {
		cv::Rect rect(iter->x, iter->y, iter->w, iter->h);
		cv::rectangle(frame, rect, cv::Scalar(0, 0, 255), 2);
		std::stringstream ss;
		ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << iter->prob;
		std::string label = "";

		ModelType model_type = getModelTypeWithValue(videoInfo.model_type);
		switch (model_type)
		{
		case ModelType_FIRE:
		{
			label = name_map_fire[iter->obj_id] + ":" + ss.str();
		}
		break;
		//case ModelType_FLOWER:
		//{
		//	label = name_rape_flower[iter->obj_id] + ":" + ss.str();
		//}
		//break;
		//case ModelType_PEST:
		//{
		//	label = name_map_pest[iter->obj_id] + ":" + ss.str();
		//}
		//break;
		//case ModelType_NORMAL:
		//	break;
		case ModelType_fire:
		{
			label = name_map_fire[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_front01:
		{
			label = name_map_front01[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_Bridge:
		{
			label = name_map_Bridge[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_CityManagement:
		{
			label = name_map_CityManagement[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_ForestFireSmokeDetection:
		{
			label = name_map_ForestFireSmokeDetection[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_InsPLAD:
		{
			label = name_map_InsPLAD[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_RoadDisaster:
		{
			label = name_map_RoadDisaster[iter->obj_id] + ":" + ss.str();
		}
		break;
		case ModelType_NORMAL:
			break;
		default:
			break;
		}

		//cout << iter->x << ", " << iter->y << ": " << label << endl;
	putTextZH(frame, label.c_str(),
			Point(iter->x, iter->y),
			Scalar(0, 0, 255), 30, "宋体");
	m_stringlabel = label;

	//emit dataready(label.c_str());
	}
	

}

void setItemData(const std::string label) {
	label;
}
void ModelProcessor::process(const cv::Mat &input, const VideoInfoData videoInfo)
{
	QMutexLocker locker(&m_mutex);

	if (!m_detector) {
		return;
	}

	try {
		cv::Mat frame = input;
		if (frame.empty()) return;

		// 进行目标检测
		std::vector<bbox_t> detections = m_detector->detect(frame, m_threshold);

			//ions = m_detector->detect(frame, m_threshold);

		// 应用非极大值抑制
		//detections = m_detector->tracking_id(detections); // 内置的NMS

		// 绘制检测结果/////////////////////////////
		drawDetections(frame, detections, videoInfo);
			if (!detections.empty())
			LY_AgriVideoPlayer::Get().GetEventRecord()->AddtableWidget(detections,  videoInfo);

		
		// 转换回RGB
		//cv::cvtColor(bgrFrame, frame, cv::COLOR_BGR2RGB);
		return;
	}
	catch (const std::exception &e) {
		qWarning() << "YOLO检测错误:" << e.what();
		return;
	}
	catch (...) {
		qCritical() << "未知的YOLO检测错误";
		return;
	}
}