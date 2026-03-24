#ifndef IPOSITIONSERVICEIMPL_H
#define IPOSITIONSERVICEIMPL_H

#include <QMutex>
#include <QObject>

#include "DataManage/AgriSevice/IAgriFertilizerPrescripyionService.h"

#include "IProj.h"


using namespace AgriSprayPrescriptionDecl;

using namespace AgriSoilTestingFertilizer;

namespace qnzkna
{
namespace AgriFertilizerPrescription
{
class IAgriFertilizerPrescriptionImpl : public IAgriFertilizerPrescriptionService
{

public:
	IAgriFertilizerPrescriptionImpl();

	virtual ~IAgriFertilizerPrescriptionImpl(void) {};

	/*画3d处方图*/
	virtual void draw3DGridMap(AgriSoilTestingFertilityPointPtrVec soilFertilityVec, double space_width, QString area_id = "");
	/*画2d处方图*/
	virtual void draw2DGridMap(QPainter &painter, IProj *pProj);
	/*处方图显隐*/
	virtual void showAndHideMap(bool isShow);
	/*清理处方图*/
	virtual void clear3dMap();
	/*生成航线*/
	virtual void generateFertilizerDiagramFlyLine(double pyfk = 4.0, double flyHeight = 5.0);
	/*清理航线数据*/
	virtual void clearFlyLine();
	/*模拟飞行*/
	virtual void staringFly(double flyHeight);
	/*模拟指令播放*/
	virtual void staringInstructionFly(double isPlayInstruction = true);
	/*生成指令集*/
	//virtual std::vector< AirInstructionPoi> generateInstructionPoint(double pyfk);


private:

	
	
private:


	

};

}
}

#endif
