
#include "AgriFertilizerPrescriptionImpl.h"

#include "LY_AgriFertilizerPrescription.h"
using namespace qnzkna::AgriFertilizerPrescription;


IAgriFertilizerPrescriptionImpl::IAgriFertilizerPrescriptionImpl()
{
	
}

/*画3d处方图*/
  void IAgriFertilizerPrescriptionImpl::draw3DGridMap(AgriSoilTestingFertilityPointPtrVec soilFertilityVec, double space_width, QString area_id)
{
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->draw3DGridMap(soilFertilityVec, space_width, area_id);
}
/*画2d处方图*/
  void IAgriFertilizerPrescriptionImpl::draw2DGridMap(QPainter &painter, IProj *pProj)
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->draw2DGridMap(painter, pProj);
  }
/*处方图显隐*/
  void IAgriFertilizerPrescriptionImpl::showAndHideMap(bool isShow)
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->showAndHideMap(isShow);
  }
/*清理处方图*/
  void IAgriFertilizerPrescriptionImpl::clear3dMap()
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->clear3dMap();
  }
/*生成航线*/
  void IAgriFertilizerPrescriptionImpl::generateFertilizerDiagramFlyLine(double pyfk, double flyHeight)
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->generateFertilizerDiagramFlyLine(pyfk, flyHeight);

  }
/*清理航线数据*/
  void IAgriFertilizerPrescriptionImpl::clearFlyLine()
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->clearFlyLine();
  }
/*模拟飞行*/
  void IAgriFertilizerPrescriptionImpl::staringFly(double flyHeight)
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->staringFly(flyHeight);
  }
/*模拟指令播放*/
  void IAgriFertilizerPrescriptionImpl::staringInstructionFly(double isPlayInstruction)
  {
	  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	  {
		  return;
	  }
	  LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->staringInstructionFly(isPlayInstruction);
  }
/*生成指令集*/
 // std::vector< AirInstructionPoi> generateInstructionPoint(double pyfk)
 // {
	//  if (LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel() == nullptr)
	//  {
	//	  return std::vector< AirInstructionPoi>();
	//  }
	//  return LY_AgriFertilizerPrescription::Get().GetFertiliVIewModel()->generateInstructionPoint(pyfk);
 //}