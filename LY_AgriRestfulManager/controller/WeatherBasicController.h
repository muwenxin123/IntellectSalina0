#ifndef WeatherBasicController_h__
#define WeatherBasicController_h__

#include <QDate>
#include <QDateTime>

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"
#include "DTO/WeatherBasicDTO.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class WeatherBasicController : public oatpp::web::server::api::ApiController {
public:
	WeatherBasicController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("GET", "/api/v1/weather/basic", getWeather) {

		QDate currentDate = QDate::currentDate();
		QString dateStr = currentDate.toString("yyyy-MM-dd");

		auto data = WeatherDataDto::createShared();
		data->timestamp = dateStr.toStdString();
		data->wind_speed = 3;
		data->wind_direction = 270;
		data->temperature = 22;
		data->humidity = 62;
		data->pressure = 94.5;
		data->rainfall = 0.0;
		  
		data->solar_radiation = 512.3;

		auto response = WeatherResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = data;

		return createDtoResponse(Status::CODE_200, response);
	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // WeatherBasicController_h__
