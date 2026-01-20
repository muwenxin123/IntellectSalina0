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

class ModelProcessor : public QObject
{
	Q_OBJECT
public:
	explicit ModelProcessor(QObject *parent = nullptr);
	~ModelProcessor();

	void setModelTitle(QString mtitle);
	bool loadModel(const QString &cfgPath, const QString &weightsPath);
    bool loadModel(const QString &cfgPath, const QString &weightsPath, const QString &namesPath);
	void process(const cv::Mat &input, const VideoInfoData videoInfo);
	bool isModelLoaded() const;

private:
	Detector* m_detector = nullptr; // Darknet YOLO检测器
	mutable QMutex m_mutex;
	QString m_currentCfgPath;
	QString m_currentWeightsPath;
	QString m_currentModelTitle;
    QString m_currentnamesPath;
    std::map<int, QString>namesContentMap;
	float m_threshold = 0.2; // 检测阈值
	float m_nms = 0.4;       // 非极大值抑制阈值
public:
	cv::Mat qimageToMat(const QImage &img);
	QImage matToQImage(const cv::Mat &mat);
	void drawDetections(cv::Mat &frame, const std::vector<bbox_t> &detections, const VideoInfoData videoInfo);
};

#endif // MODELPROCESSOR_H
