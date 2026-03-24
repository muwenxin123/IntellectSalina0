#include "ITask.h"

#include "IAgriRestfulManagerActivator.h"

#include "../LY_AgriFertilizerPrescription/LY_AgriFertilizerPrescription.h"

void ITask::cleanUp()
{
	auto agriFertilizePescViewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
	
	// 清理分布图
	auto agriSprayService = IAgriRestfulManagerActivator::Get().getPIAgriDataAnalysisPredictervice();
	agriSprayService->cleanupMap();

	// 清理分布图
	auto agriSoilTestingService = IAgriRestfulManagerActivator::Get().getIAgriSoilTestingAnalysisService();
	agriSoilTestingService->cleanupMap();

	// 清理巡飞数据
	agriSprayService->cleanupMutilspectralMap();
	agriSprayService->hideResultLegend();

	// 清理喷药数据
	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();
		agriSprayPrescriptService->clearn3DflyData();
		agriSprayPrescriptService->clearSprayDiagramData();


	}, Qt::QueuedConnection);

	// 清理施肥数据
	QMetaObject::invokeMethod(agriFertilizePescViewModel, [&]() {

		auto viewModel = LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel();
		if (viewModel){
			viewModel->clear3dMap();
			viewModel->clearFlyLine();
		}

	}, Qt::QueuedConnection);
}
