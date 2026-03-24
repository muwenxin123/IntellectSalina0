#ifndef WorkResultDTO_h__
#define WorkResultDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class DiseaseRiskDto : public oatpp::DTO {
	DTO_INIT(DiseaseRiskDto, DTO)

	DTO_FIELD(Float32, rust);        // 锈病风险
	DTO_FIELD(Float32, blight);      // 大斑病风险
	DTO_FIELD(Float32, cornBorer);   // 玉米螟风险
};

// 巡田指标
class PatrolRateDto : public oatpp::DTO {
	DTO_INIT(PatrolRateDto, DTO)

	DTO_FIELD(Float64, inspection_coverage);
	DTO_FIELD(Float64, healthy_vegetation_rate);
	DTO_FIELD(Int32, abnormal_event_detection);

	DTO_FIELD(String, fieldId);                          // 地块编号
	DTO_FIELD(String, fieldName);                        // 地块名称
	DTO_FIELD(String, fieldType);                        // 地块类型
	DTO_FIELD(String, location);                         // 地块位置
	DTO_FIELD(String, cropType);                         // 作物类型
	DTO_FIELD(Float64, area_mu);                         // 面积（亩）
	DTO_FIELD(String, growthStage);                      // 生育期
	DTO_FIELD(String, flightDate);                       // 巡检时间

	// 多光谱指数与生长评估
	DTO_FIELD(Float64, ndvi);
	DTO_FIELD(Float64, gndvi);
	DTO_FIELD(Float64, ndre);
	DTO_FIELD(Float64, biomassEstimate_kgPerMu);         // 单亩生物量估算
	DTO_FIELD(Float64, maturityScore);                   // 成熟度 [0-1]
	DTO_FIELD(Float64, stressLevel);                     // 胁迫水平 [0-1]

	// 病虫害风险评估
	DTO_FIELD(Object<DiseaseRiskDto>, diseaseRisk);
	DTO_FIELD(Boolean, needFertilization);
	DTO_FIELD(String, recommendedFertilizerType);
	DTO_FIELD(Float64, fertilizerRate_kgPerMu);
	DTO_FIELD(String, fertilizerApplicationMethod);
	DTO_FIELD(Float64, fertilizerTotalAmount_kg);

	DTO_FIELD(Boolean, needPesticide);
	DTO_FIELD(String, targetPest);
	DTO_FIELD(String, recommendedPesticide);
	DTO_FIELD(Float64, pesticideRate_mlPerMu);
	DTO_FIELD(String, pesticideApplicationMethod);
	DTO_FIELD(String, urgencyLevel);
	DTO_FIELD(Float64, pesticideTotalAmount_ml);

	DTO_FIELD(String, suggestionSummary);
};

// 防治效果指标
class PestControlRateDto : public oatpp::DTO {
	DTO_INIT(PestControlRateDto, DTO)

	DTO_FIELD(Float64, pest_control_rate);
	DTO_FIELD(Float64, reduction_rate);
	DTO_FIELD(Float64, disease_index_drop);
	DTO_FIELD(Float64, medicine_coverage);
};

// 施肥效果指标
class ReductionRateDto : public oatpp::DTO {
	DTO_INIT(ReductionRateDto, DTO)

	DTO_FIELD(Float64, nutrient_utilization);
	DTO_FIELD(Float64, growth_promotion_rate);
	DTO_FIELD(Float64, fertilization_coverage);
};

// 数据主体
class ResultDataDto : public oatpp::DTO {
	DTO_INIT(ResultDataDto, DTO)

	DTO_FIELD(Float64, area_per_hour);
	DTO_FIELD(Float64, chemical_efficiency);
	DTO_FIELD(Float64, comprehensive_energy);
	DTO_FIELD(Int32, result_flag);
	DTO_FIELD(Object<PatrolRateDto>, patrol_rate);
	DTO_FIELD(Object<PestControlRateDto>, pest_control_rate);
	DTO_FIELD(Object<ReductionRateDto>, reduction_rate);
};

// 响应对象
class ResultResponseDto : public oatpp::DTO {
	DTO_INIT(ResultResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<ResultDataDto>, data);
};

#include OATPP_CODEGEN_END(DTO)

#endif // WorkResultDTO_h__
