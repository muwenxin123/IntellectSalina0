#ifndef IAGRIYOLORECOGNIZE_H_
#define IAGRIYOLORECOGNIZE_H_

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include "IRecognizeInterface.h"

namespace recognize
{
	class AgriYoloRecognize : public IRecognizeInterface
	{
	public:
		AgriYoloRecognize();
		virtual ~AgriYoloRecognize();

		virtual bool Recognize(const QImage &originImage, QImage &recoginzedImage, QString &recoginzedName, float threshold = 0.2) override;

	private:
		cv::Mat QImageToMat(const QImage &image);

	private:
		std::map<int, std::string> name_map{
				{ 0, "玉米粗缩病" },
				{ 1, "玉米锈病" },
				{ 2, "玉米大斑病" },
				{ 3, "玉米小斑病" },
				{ 4, "玉米纹枯病" },
				{ 5, "玉米丝黑穗病" },
				{ 6, "玉米瘤黑粉病" },
				{ 7, "玉米茎腐病" },
				{ 8, "玉米细菌性茎腐病" },
				{ 9, "玉米灰斑病" },
				{ 10, "玉米褐斑病" },
				{ 11, "玉米链格孢叶枯病" },
				{ 12, "玉米弯孢叶斑病" },
				{ 13, "玉米炭疽病" },
				{ 14, "玉米眼斑病" },
				{ 15, "玉米全蚀病" },
				{ 16, "玉米霜霉病" },
				{ 17, "玉米干腐病" },
				{ 18, "玉米赤霉病" },
				{ 19, "玉米矮花叶病毒病" },
				{ 20, "玉米条纹矮缩病" },
				{ 21, "玉米空秆病" },
				{ 22, "玉米顶腐病" },
				{ 23, "蚜虫" },
				{ 24, "棉铃虫" },
				{ 25, "甜菜夜蛾" },
				{ 26, "斜纹夜蛾" },
				{ 27, "玉米螟" },
				{ 28, "粘虫" },
				{ 29, "草地贪夜蛾" },
				{ 30, "二点委夜蛾" },
				{ 31, "蓟马" },
				{ 32, "地老虎" },
				{ 33, "玉米向日葵粉蚧" },
				{ 34, "金龟子" },
				{ 35, "东亚飞蝗" },
				{ 36, "中华蚱蜢" },
				{ 37, "稻绿蝽" }
		};
	};
}
#endif  // IAgriYoloRecognize_H