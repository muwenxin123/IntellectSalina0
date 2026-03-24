#include <QDebug>

#include "UAVSprayTask.h"

#include "IAgriRestfulManagerActivator.h"

#include "../LY_AgriFertilizerPrescription/LY_AgriFertilizerPrescription.h"

UAVSprayTask::UAVSprayTask()
	: ITask()
{
}

UAVSprayTask::~UAVSprayTask()
{
}

int UAVSprayTask::doWork()
{
	// 开始工作
#ifdef _INC_WINDOWS
	Sleep(rand() % 300);
#endif
	qDebug() << "spray task : " << id_ << " doing.";

	// 清理上次任务残留数据
	ITask::cleanUp();

	std::vector<QPointF> vboundPoings;
	auto agriSprayService = IAgriRestfulManagerActivator::Get().getPIAgriDataAnalysisPredictervice();
	
	// 生成分布图
	if (agriSprayService)
	{
		agriSprayService->getActiveFieldBounds(vboundPoings);
		agriSprayService->generatePestDistributeMap();
	}

	while (isWaitRequired())
	{
		// 等待任务执行
		Sleep(1 * 1000);
	}

	// 等待2s 生成处方图
	auto agriFertilizePescViewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
	auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();

	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriSprayPrescriptService)
		{
			agriSprayPrescriptService->generateSprayPrescriptionDiagram(vboundPoings, "c859aaa4-731e-4602-aacb-1f7dd6c09b33");
		}
	}, Qt::QueuedConnection);

	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		if (agriSprayPrescriptService)
		{
			QString spray_id = "c859aaa4-731e-4602-aacb-1f7dd6c09b33";
			double pyfk = 4.0;
			double flyHeight = 5.0;
			std::vector<Poi> flyPoints;
			double flyDisance;
			agriSprayPrescriptService->generateSprayDiagramFlyLine(vboundPoings, spray_id, pyfk, flyHeight, false, flyPoints, flyDisance);
			agriSprayPrescriptService->showSprayDiagramData(false);
		}

		// 等待2s 等待模拟飞行
		Sleep(2 * 1000);

		if (agriSprayPrescriptService)
		{
			agriSprayPrescriptService->flyRouteSimulPlayback(5.0);
			agriSprayPrescriptService->playInstructionFlying();
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
				break;
			}
			Sleep(0.1 * 1000);
		}
	}

	return 0;
}

int UAVSprayTask::onCanceled(void)
{
	// 任务被取消
	qDebug() << "spray task : " << id_ << " canceled.";
	return 0;
}

int UAVSprayTask::onCompleted(int)
{
	// 任务已完成，返回码为code
	qDebug() << "spray task : " << id_ << " completed.";
	return 0;
}