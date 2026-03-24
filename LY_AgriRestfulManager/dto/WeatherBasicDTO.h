#ifndef WeatherBasicDTO_h__
#define WeatherBasicDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

// 天气信息数据
class WeatherDataDto : public oatpp::DTO {
	DTO_INIT(WeatherDataDto, DTO)

	DTO_FIELD(String, timestamp);
	DTO_FIELD(Float64, wind_speed);
	DTO_FIELD(Int32, wind_direction);
	DTO_FIELD(Float64, temperature);
	DTO_FIELD(Float64, humidity);
	DTO_FIELD(Float64, pressure);
	DTO_FIELD(Float64, rainfall);
	DTO_FIELD(Float64, solar_radiation);
};

// 响应对象
class WeatherResponseDto : public oatpp::DTO {
	DTO_INIT(WeatherResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<WeatherDataDto>, data);
};

#include OATPP_CODEGEN_END(DTO)

#endif // WeatherBasicDTO_h__
