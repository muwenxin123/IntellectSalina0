#ifndef UAVFertilizeDTO_h__
#define UAVFertilizeDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

// 环境参数
class EnvironmentDto : public oatpp::DTO {
	DTO_INIT(EnvironmentDto, DTO)

	DTO_FIELD(Int32, temp);
	DTO_FIELD(Float64, windSpeed);
};

// 土壤信息
class SoilInfoDto : public oatpp::DTO {
	DTO_INIT(SoilInfoDto, DTO)

	DTO_FIELD(Float64, N);
	DTO_FIELD(Float64, P);
	DTO_FIELD(Float64, K);
	DTO_FIELD(Float64, pH);
	DTO_FIELD(Float64, SOM);
};

// 施肥方案
class ChemicalSchemeDto : public oatpp::DTO {
	DTO_INIT(ChemicalSchemeDto, DTO)

	DTO_FIELD(Float64, lat);
	DTO_FIELD(Float64, lon);
	DTO_FIELD(Float64, pygd);
	DTO_FIELD(Float64, pyfk);
	DTO_FIELD(String, pysb);
	DTO_FIELD(String, pzlx);
	DTO_FIELD(String, pyfs);
	DTO_FIELD(String, pykg);
	DTO_FIELD(Float64, rote_speed);
	DTO_FIELD(Float64, flow_speed);
	DTO_FIELD(Float64, droplet_size);
	DTO_FIELD(Int32, penzui_size);
	DTO_FIELD(Int32, point_status);
};

// 任务数据
class FertilizeTaskDto : public oatpp::DTO {
	DTO_INIT(FertilizeTaskDto, DTO)

	DTO_FIELD(Int32, id);
	DTO_FIELD(String, spray_guid);
	DTO_FIELD(String, crop_type);
	DTO_FIELD(String, growth_stage);
	DTO_FIELD(String, pest_type);
	DTO_FIELD(String, severity_level);
	DTO_FIELD(Float64, tank_capacity);
	DTO_FIELD(Object<EnvironmentDto>, environment);
	DTO_FIELD(Object<SoilInfoDto>, soil_info);
	DTO_FIELD(Float64, total_area);
	DTO_FIELD(List<Object<ChemicalSchemeDto>>, chemical_scheme);
	DTO_FIELD(Float64, total_amount);
	DTO_FIELD(Float64, flight_params);
	DTO_FIELD(Float64, estimated_time);
	DTO_FIELD(Float64, max_flighttime);
};

// 响应对象
class FertilizeTaskResponseDto : public oatpp::DTO {
	DTO_INIT(FertilizeTaskResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<FertilizeTaskDto>, data);
};

#include OATPP_CODEGEN_END(DTO)

#endif // UAVFertilizeDTO_h__
