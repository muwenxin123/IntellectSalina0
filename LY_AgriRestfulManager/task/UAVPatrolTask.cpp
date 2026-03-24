#include <QDebug>

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgGA/AnimationPathManipulator>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osgEarth/GeoTransform>

#include "map3d/IEarth.h"

#include "UAVPatrolTask.h"

#include "LY_AgriRestfulManagerInterface.h"

#include "IAgriRestfulManagerActivator.h"
#include "../LY_AgriFertilizerPrescription/LY_AgriFertilizerPrescription.h"


void quatToEuler(const osg::Quat& q, double& heading, double& pitch, double& roll)
{
	// 使用 ZYX 顺序（航向为绕Z轴，俯仰为Y，滚转为X）
	double ysqr = q.y() * q.y();

	// roll (x-axis rotation)
	double t0 = +2.0 * (q.w() * q.x() + q.y() * q.z());
	double t1 = +1.0 - 2.0 * (q.x() * q.x() + ysqr);
	roll = std::atan2(t0, t1);

	// pitch (y-axis rotation)
	double t2 = +2.0 * (q.w() * q.y() - q.z() * q.x());
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	pitch = std::asin(t2);

	// heading (z-axis rotation)
	double t3 = +2.0 * (q.w() * q.z() + q.x() * q.y());
	double t4 = +1.0 - 2.0 * (ysqr + q.z() * q.z());
	heading = std::atan2(t3, t4);
}


class GeoPathCallback : public osg::NodeCallback
{
public:
	GeoPathCallback(osg::ref_ptr<osg::AnimationPath> path, osgEarth::MapNode* mapNode, const osgEarth::GeoPoint& startGeo)
		: _path(path), _mapNode(mapNode), _startGeo(startGeo), _startTime(-1.0) {}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (!_path.valid() || !_mapNode) return;

		if (_startTime < 0.0)
			_startTime = nv->getFrameStamp()->getSimulationTime();

		double currentTime = nv->getFrameStamp()->getSimulationTime() - _startTime;

		osg::Matrix matrix;
		if (_path->getMatrix(currentTime, matrix))
		{
			osg::Vec3 offset = matrix.getTrans(); // 相对偏移量，单位米

			osg::Quat rot = matrix.getRotate();
			double heading, pitch, roll;
			quatToEuler(rot, heading, pitch, roll);

			// 转为角度
			heading = osg::RadiansToDegrees(heading);
			pitch = osg::RadiansToDegrees(pitch);
			roll = osg::RadiansToDegrees(roll);

			// 将初始地理点转换为世界坐标
			osg::Vec3d worldXYZ;
			_startGeo.toWorld(worldXYZ);

			// 添加偏移
			osg::Vec3d newXYZ = worldXYZ + offset;

			// 再转换回地理坐标
			osgEarth::GeoPoint newGeo;
			newGeo.fromWorld(_mapNode->getMapSRS(), newXYZ);

#if 0
			// 设置新位置
			auto geoTransform = dynamic_cast<osgEarth::GeoTransform*>(node);
			if (geoTransform)
			{
				geoTransform->setPosition(newGeo);
			}
#else
			// 更新相机镜头
			// 104.0906,30.9458,600,-30,-25,190000
			QString str = QString("CenterAt(%1, %2, %3, %4, %5, %6)")
				.arg(offset.x(), 0, 'f', 7)
				.arg(offset.y(), 0, 'f', 7)
				.arg(offset.z(), 0, 'f', 7)
				.arg(heading, 0, 'f', 7)
				.arg(pitch, 0, 'f', 7)
				.arg(190000);

			LY_AgriRestfulManagerInterface::Get().Script(str.toLocal8Bit());
#endif
		}

		traverse(node, nv);
	}

private:
	osg::ref_ptr<osg::AnimationPath> _path;
	osgEarth::MapNode* _mapNode;
	osgEarth::GeoPoint _startGeo;
	double _startTime;
};


// 创建漫游路径
osg::ref_ptr<osg::AnimationPath> createAnimationPath(const osg::Vec3d& startPos, const osg::Vec3d& endPos, double time)
{
	auto pAnimationPath = new osg::AnimationPath;
	pAnimationPath->setLoopMode(osg::AnimationPath::LOOP);

	auto timeDeta = 1;

	const auto loopCount = time / timeDeta;

	// 这里假定模型在x、y、z方向上运动的速度都一样且是匀速直线运动
	auto xStep = (endPos.x() - startPos.x()) / loopCount; // 每个时间段前进的距离
	auto yStep = (endPos.y() - startPos.y()) / loopCount;
	auto zStep = (endPos.z() - startPos.z()) / loopCount;

	double angle = 0.0;
	double twoPi = 2 * osg::PI;
	osg::Vec3d lastPos = startPos;
	osg::Vec3d pos = startPos;
	for (auto timeOffset = 0.0; timeOffset <= time; timeOffset += timeDeta)
	{
		if (0.0 < timeOffset)
		{
			pos.x() = pos.x() + xStep;
			pos.y() = pos.y() + yStep;
			pos.z() = pos.z() + zStep;

			auto xOffset = pos.x() - lastPos.x();
			auto yOffset = pos.y() - lastPos.y();
			angle = atan2f(yOffset, xOffset); // 算出偏转方向
			angle = twoPi + angle;
		}

		osg::Quat roateByZAxis(0/*angle*/, osg::Vec3(0, 0, 1));
		osg::AnimationPath::ControlPoint ctrlPoint(pos, roateByZAxis); // 动画控制点
		pAnimationPath->insert(timeOffset, ctrlPoint);
		lastPos = pos;
	}

	// 保存动画路径到文件
	osgDB::ofstream out("cow_animationpath.txt");
	pAnimationPath->write(out);

	return pAnimationPath;
}


UAVPatrolTask::UAVPatrolTask()
	: ITask()
{
}

UAVPatrolTask::~UAVPatrolTask()
{
}

int UAVPatrolTask::doWork()
{
	// 开始工作
#ifdef _INC_WINDOWS
	Sleep(rand() % 300);
#endif
	qDebug() << "patrol task : " << id_ << " doing.";
	// 
	//QString str = "CenterAt(104.0906, 30.9458, 600, -30, -25, 190000)";
	//LY_AgriRestfulManagerInterface::Get().Script(str.toLocal8Bit());

	/*
	auto agriFertilizePescViewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
	auto agriSoilTestingService = IAgriRestfulManagerActivator::Get().getIAgriSoilTestingAnalysisService();
	if (agriSoilTestingService)
	{
		QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

			//auto pCow = osgDB::readRefNodeFile(R"(cow.osg)");
			//if (nullptr == pCow)
			//{
			//	OSG_WARN << "cow node is nullpr!" << std::endl;
			//	return 1;
			//}

			osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));

			//104.0906, 30.9458, 600, -30, -25, 190000

			// 初始地理位置
			auto initPos = osg::Vec3d(104.0901304, 30.9468978, 513); // lon, lat, alt (单位: 度, 度, 米)
			osgEarth::GeoPoint startPoint(mapNode->getMapSRS(), initPos.x(), initPos.y(), initPos.z(), osgEarth::ALTMODE_ABSOLUTE);

			// 创建路径
			auto path = createAnimationPath(initPos, osg::Vec3d(104.0886584, 30.9465486, 513), 10.0);

			// 创建 GeoTransform 并设置起始位置
			osg::ref_ptr<osgEarth::GeoTransform> geoTransform = new osgEarth::GeoTransform();
			geoTransform->setPosition(startPoint);
			//geoTransform->addChild(pCow);
			geoTransform->setUpdateCallback(new GeoPathCallback(path, mapNode, startPoint));

			mapNode->addChild(geoTransform);

		}, Qt::QueuedConnection);
	}
	*/

	// 清理上次任务残留数据
	ITask::cleanUp();

	std::vector<QPointF> vboundPoings;
	auto agriSprayService = IAgriRestfulManagerActivator::Get().getPIAgriDataAnalysisPredictervice();
	if (agriSprayService)
	{
		agriSprayService->getActiveFieldBounds(vboundPoings);
	}

	// 生成处方图
	auto agriFertilizePescViewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
	auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();

	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriSprayPrescriptService)
		{
			agriSprayPrescriptService->generateSprayPrescriptionDiagram(vboundPoings, "c859aaa4-731e-4602-aacb-1f7dd6c09b33", 4.0, 5, true);
		}

		if (agriSprayService)
		{
			agriSprayService->showTaskInfotLegend();
		}

	}, Qt::QueuedConnection);

	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriSprayPrescriptService)
		{
			QString spray_id = "c859aaa4-731e-4602-aacb-1f7dd6c09b33";
			double pyfk = 4.0;
			double flyHeight = 30.0;
			std::vector<Poi> flyPoints;
			double flyDisance;
			agriSprayPrescriptService->generateSprayDiagramFlyLine(vboundPoings, spray_id, pyfk, flyHeight, false, flyPoints, flyDisance);
			agriSprayPrescriptService->showSprayDiagramData(false);
		}
	}, Qt::QueuedConnection);

	while (isWaitRequired())
	{
		// 等待任务执行
		Sleep(1 * 1000);
	}

	// 模拟飞行
	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriSprayPrescriptService)
		{
			agriSprayPrescriptService->flyRouteSimulPlayback(30.0, true);
			agriSprayPrescriptService->playInstructionFlying(false);
		}
	}, Qt::QueuedConnection);

	// 查询任务状态
	if (agriSprayPrescriptService)
	{
		UavStatus status;
		bool isFinshed = false;
		while (true)
		{
			status = agriSprayPrescriptService->getUavStatus();
			if (0 == status.task_stage.compare("finished"))
			{
				isFinshed = true;
			}

			if (isFinshed && (0 == status.stage.compare("landed")))
			{
				Sleep(3 * 1000);

				// 清理任务
				QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

					auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();
					agriSprayPrescriptService->clearSprayDiagramData();
					agriSprayPrescriptService->clearn3DflyData();
				}, Qt::QueuedConnection);

				// 绘制多光谱图像
				if (agriSprayService)
				{
					agriSprayService->generateMutilspectralMap();
					agriSprayService->showResultLegend();
				}

				break;
			}
			Sleep(0.1 * 1000);
		}
	}

	return 0;
}

int UAVPatrolTask::onCanceled(void)
{
	// 任务被取消
	qDebug() << "patrol task : " << id_ << " canceled.";
	return 0;
}

int UAVPatrolTask::onCompleted(int)
{
	// 任务已完成，返回码为code
	qDebug() << "patrol task : " << id_ << " completed.";
	return 0;
}