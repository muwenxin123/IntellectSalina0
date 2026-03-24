#ifndef MODELPROCESSOR_H
#define MODELPROCESSOR_H

#define OPENCV
#include <QObject>
#include <QImage>
#include <QMutex>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "yolo_v2_class.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <direct.h>
#include <io.h>
#include <iosfwd>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "videoconfig.h"
#include "EventRecord.h"
class ModelProcessor : public QObject
{
	Q_OBJECT
public:
	explicit ModelProcessor(QObject *parent = nullptr);
	~ModelProcessor();

	bool loadModel(const QString &cfgPath, const QString &weightsPath);
	void process(const cv::Mat &input, const VideoInfoData videoInfo);
	bool isModelLoaded() const;

private:
	Detector* m_detector = nullptr; // Darknet YOLO检测器
	mutable QMutex m_mutex;
	QString m_currentCfgPath;
	QString m_currentWeightsPath;
	float m_threshold = 0.5; // 检测阈值
	float m_nms = 0.4;       // 非极大值抑制阈值
	//EventRecord* m_EventRecord = nullptr;
	cv::Mat qimageToMat(const QImage &img);
	QImage matToQImage(const cv::Mat &mat);
	void drawDetections(cv::Mat &frame, const std::vector<bbox_t> &detections, const VideoInfoData videoInfo);
	std::string m_stringlabel;

signals:
	void dataready(const char* str);
};

#endif // MODELPROCESSOR_H
