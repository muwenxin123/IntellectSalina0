#ifndef WorkResultController_h__
#define WorkResultController_h__

#include <QDate>
#include <QDateTime>
#include <QRandomGenerator>

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/WorkResultDTO.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class WorkResultController : public oatpp::web::server::api::ApiController {
public:
	WorkResultController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("GET", "/api/v1/uav/work/result", postWorkResult) {

		auto patrol = PatrolRateDto::createShared();

		patrol->inspection_coverage = 93.2;
		patrol->healthy_vegetation_rate = 87.6;
		patrol->abnormal_event_detection = 12;

		patrol->fieldId = u8"PZ-001";
		patrol->fieldName = u8"彭州1#盐田";
		patrol->fieldType = u8"水浇地";
		patrol->location = u8"109.1233432,39.23211";
		patrol->cropType = u8"玉米";
		patrol->area_mu = 7.70f;
		patrol->growthStage = u8"大喇叭口";

		QDate currentDate = QDate::currentDate();
		QString dateStr = currentDate.toString("yyyy-MM-dd");
		patrol->flightDate = dateStr.toStdString();

		patrol->ndvi = 0.74f;
		patrol->gndvi = 0.69f;
		patrol->ndre = 0.51f;
		patrol->biomassEstimate_kgPerMu = 600.0f;
		patrol->maturityScore = 0.75f;
		patrol->stressLevel = 0.28f;

		auto disease = DiseaseRiskDto::createShared();
		disease->rust = 0.22f;
		disease->blight = 0.27f;
		disease->cornBorer = 0.68f;
		patrol->diseaseRisk = disease;

		// 获取当前时间戳（秒）
		qint64 timestamp = QDateTime::currentSecsSinceEpoch();
		QRandomGenerator rng(static_cast<quint32>(timestamp & 0xFFFFFFFF));
		bool flag = (rng.bounded(2) == 1);

		patrol->needFertilization = flag;
		patrol->recommendedFertilizerType = u8"氮钾复合肥";
		patrol->fertilizerRate_kgPerMu = 20.0f;
		patrol->fertilizerApplicationMethod = u8"水肥一体化";
		patrol->fertilizerTotalAmount_kg = 200.0f;

		patrol->needPesticide = !flag;
		patrol->targetPest = u8"玉米螟";
		patrol->recommendedPesticide = u8"甲维·茚虫威（10%）";
		patrol->pesticideRate_mlPerMu = 35.0f;
		patrol->pesticideApplicationMethod = u8"无人机喷雾";
		patrol->urgencyLevel = u8"高";
		patrol->pesticideTotalAmount_ml = 350.0f;

		if (flag){
			patrol->suggestionSummary = u8"玉米进入灌浆期，检测显示NDVI偏低，生物量不足，建议补施氮钾复合肥（20kg/亩）以促进粒重形成，暂未发现显著病虫害风险。";
		}else{
			patrol->suggestionSummary = u8"检测到较高玉米螟虫害风险（0.68），建议使用甲维·茚虫威（10%）按35ml/亩配比进行无人机喷雾，尽快执行防控。当前作物长势良好，无需施肥。";
		}

		auto pest = PestControlRateDto::createShared();
		pest->pest_control_rate = 87.5;
		pest->reduction_rate = 90.2;
		pest->disease_index_drop = 60.0;
		pest->medicine_coverage = 92.3;

		auto reduction = ReductionRateDto::createShared();
		reduction->nutrient_utilization = 68.5;
		reduction->growth_promotion_rate = 72.0;
		reduction->fertilization_coverage = 91.0;

		auto data = ResultDataDto::createShared();
		data->area_per_hour = 65.0;
		data->chemical_efficiency = 43.5;
		data->comprehensive_energy = 35.2;
		data->result_flag = 1;
		data->patrol_rate = patrol;
		data->pest_control_rate = pest;
		data->reduction_rate = reduction;

		auto response = ResultResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = data;

		return createDtoResponse(Status::CODE_200, response);
	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // WorkResultController_h__
