#define OPENCV

#include <QString>
#include <QDebug>
#include <QBuffer>
#include <sstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "yolo_v2_class.hpp"
#include "QCString.h"
#include "LyMessageBox.h"
#include "AgriYoloRecognize.h"
#include "PutTextZH.h"

const QString cfg_file = "../3rdParty/bin/yolo/nongye.cfg";
const QString weight_file = "../3rdParty/bin/yolo/nongye.weights";


recognize::AgriYoloRecognize::AgriYoloRecognize()
{

}

recognize::AgriYoloRecognize::~AgriYoloRecognize()
{

}

bool recognize::AgriYoloRecognize::Recognize(const QImage &originImage, QImage &recoginzedImage, QString &recoginzedName, float threshold/* = 0.2*/)
{
	// 配置文件校验
	auto& isFileExists = [](const QString &relativePath)->bool {
		QString appDir = QCoreApplication::applicationDirPath();
		QString absoluteFilePath = QDir(appDir).filePath(relativePath);
		absoluteFilePath = QDir(absoluteFilePath).canonicalPath();// 规范化路径，避免 `..` 等相对路径影响
		return QFile::exists(absoluteFilePath);
	};

	if (!isFileExists(cfg_file) || !isFileExists(weight_file))
	{
		qDebug() << tr2("创建检测对象配置文件获取失败! ") << QString("cfg_file: %1, weight_file: %2")
			.arg(cfg_file)
			.arg(weight_file);
		return false;
	}

	// 创建检测对象
	Detector my_detector(cfg_file.toStdString(), weight_file.toStdString(), 0);

	cv::Mat matImage = QImageToMat(originImage);

	if (matImage.empty()) return false;

	//yolo检测视频帧
	std::vector<bbox_t> result_vec = my_detector.detect(matImage, threshold);

	for (auto iter = result_vec.begin(); iter != result_vec.end(); ++iter) {
		cv::Rect rect(iter->x, iter->y, iter->w, iter->h);
		cv::rectangle(matImage, rect, cv::Scalar(0, 255, 0), 2);

		std::stringstream ss;
		ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << iter->prob;

		std::string label = "";
		auto it = name_map.find(iter->obj_id);
		if (it != name_map.end())
		{
			label = it->second + ":" + ss.str();
			recoginzedName = tr2(it->second.c_str());
		}

		if (label.length() > 0)
		{
			int fontSize = 35;
			putTextZH(matImage, label.c_str(),
				Point(iter->x, iter->y - fontSize),
				Scalar(0, 0, 255), fontSize, "宋体");

			QImage qImage = QImage((const unsigned char*)(matImage.data),
				matImage.cols,
				matImage.rows,
				matImage.step,
				QImage::Format_RGB888);

			recoginzedImage = qImage.rgbSwapped().copy();
			return true;
			/*
				QPixmap recognitionPixmap = QPixmap::fromImage(qImage1);
				QBuffer buffer;
				buffer.open(QIODevice::WriteOnly);
				recognitionPixmap.save(&buffer, "PNG");  // 使用PNG格式保存图像到buffer
				buffer.close();
				QByteArray byteArray = buffer.buffer();  // 获取完整的字节数组
			*/
		}
	}

	return false;
}


cv::Mat recognize::AgriYoloRecognize::QImageToMat(const QImage &image)
{
	Mat mat;
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		break;

	case QImage::Format_RGB32:
		mat = Mat(image.height(), image.width(), CV_8UC3);
		for (int y = 0; y < image.height(); ++y) {
			const uchar *srcLine = image.constScanLine(y);
			cv::Vec3b *dstLine = mat.ptr<cv::Vec3b>(y);
			for (int x = 0; x < image.width(); ++x) {
				QRgb pixel = reinterpret_cast<const QRgb*>(srcLine)[x];
				dstLine[x][0] = qBlue(pixel);  // B
				dstLine[x][1] = qGreen(pixel); // G
				dstLine[x][2] = qRed(pixel);   // R
			}
		}
		break;
	case QImage::Format_RGB888:
		mat = Mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		cvtColor(mat, mat, COLOR_RGB2BGR);
		break;
	default:
		qWarning() << "Unsupported image format";
		break;
	}
	return  mat;
}
