#include <QDebug>

#include "UAVFertilizeTask.h"

#include "IAgriRestfulManagerActivator.h"

#include "../LY_AgriFertilizerPrescription/LY_AgriFertilizerPrescription.h"

UAVFertilizeTask::UAVFertilizeTask()
	: ITask()
{
}

UAVFertilizeTask::~UAVFertilizeTask()
{
}

int UAVFertilizeTask::doWork()
{
	// 开始工作
#ifdef _INC_WINDOWS
	Sleep(rand() % 300);
#endif
	std::cout << "task : " << id_ << " doing." << std::endl;

	// 清理上次任务残留数据
	ITask::cleanUp();

	auto agriFertilizePescViewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
	auto agriSoilTestingService = IAgriRestfulManagerActivator::Get().getIAgriSoilTestingAnalysisService();

	AgriSoilFertilityDistributeMapPtr distributeMap = std::make_shared<AgriSoilFertilityDistributeMap>();
	distributeMap->guid = u8"de09f514-20de-4d66-bc0b-33d72bb0457b";
	distributeMap->name = u8"分布图_P01";
	distributeMap->dataset_id = u8"dd32ac08-a6d5-427c-b7cb-2ca8995f7e5d";
	distributeMap->crop_type = 0;
	distributeMap->growth_stage = 0;
	distributeMap->field_area = 8;
	distributeMap->sample_type = 2;
	distributeMap->sample_mode = 3;
	distributeMap->collect_count = 53;
	distributeMap->grid_unit = 1;
	distributeMap->diffuse_layer = 5;
	distributeMap->distribute_statistic = u8"600,193,195,144,228";
	distributeMap->map_type = 1;
	distributeMap->valid_identify = 4;

	AgriSoilTestingFertilityPointPtrVec outSoilFertilityPointVec;

	std::vector<QPointF> vboundPoings;
	auto agriSprayService = IAgriRestfulManagerActivator::Get().getPIAgriDataAnalysisPredictervice();
	if (agriSprayService)
	{
		agriSprayService->getActiveFieldBounds(vboundPoings);
	}

	// 生成分布图
	if (agriSoilTestingService)
	{
		QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {
			agriSoilTestingService->GetSoilFertilityDistribute(distributeMap, outSoilFertilityPointVec, vboundPoings);
		}, Qt::QueuedConnection);
		
		agriSoilTestingService->generateSoilTestingMap();
		// 等待8s生成
		Sleep(8 * 1000);
	}

	while (isWaitRequired())
	{
		// 等待任务执行
		Sleep(1 * 1000);
	}

	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriFertilizePescViewModel)
		{
			QString area_id = u8"彭州盐田1";
			agriFertilizePescViewModel->draw3DGridMap(outSoilFertilityPointVec, 4.0, area_id);
		}
	}, Qt::QueuedConnection);
	
	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {
		
		if (agriFertilizePescViewModel)
		{
			agriFertilizePescViewModel->generateFertilizerDiagramFlyLine();
			agriFertilizePescViewModel->showAndHideMap(false);
			// 等待3s 等待模拟飞行
			Sleep(3 * 1000);
		}

		if (agriFertilizePescViewModel)
		{
			agriFertilizePescViewModel->staringFly(5.0);
		}
	}, Qt::QueuedConnection);


	// 查询任务状态
	auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();
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
				// ...
				break;
			}

			Sleep(0.1 * 1000);
		}
	}

	return 0;
}

int UAVFertilizeTask::onCanceled(void)
{
	// 任务被取消
	std::cout << "task : " << id_ << " canceled." << std::endl;
	return 0;
}

int UAVFertilizeTask::onCompleted(int)
{
	// 任务已完成，返回码为code
	std::cout << "task : " << id_ << " completed." << std::endl;
	return 0;
}