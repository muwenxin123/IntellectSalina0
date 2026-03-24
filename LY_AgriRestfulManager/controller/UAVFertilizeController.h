#ifndef UAVFertilizeController_h__
#define UAVFertilizeController_h__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/UAVFertilizeDTO.h"

#include "task/UAVFertilizeTask.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UAVFertilizeController : public oatpp::web::server::api::ApiController {
public:
	/**
	* Constructor with object mapper.
	* @param apiContentMappers - mappers used to serialize/deserialize DTOs.
	*/
	UAVFertilizeController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("POST", "/api/v1/uav/fertilization/plan", postFertilizePlan) {

		std::shared_ptr<ITask> itask = LY_AgriRestfulManager::Get().isExistTaskProcess();
		if (itask)
		{
			auto response = FertilizeTaskResponseDto::createShared();
			response->code = -1;
			response->message = "error: exits running task";

			return createDtoResponse(Status::CODE_200, response);
		}

		// 添加施肥任务
		std::shared_ptr<UAVFertilizeTask> fertilizeTask = std::shared_ptr<UAVFertilizeTask>(new UAVFertilizeTask());
		LY_AgriRestfulManager::Get().addTask(fertilizeTask);

		auto env = EnvironmentDto::createShared();
		env->temp = 28;
		env->windSpeed = 2.0;

		auto soil = SoilInfoDto::createShared();
		soil->N = 180.0;
		soil->P = 30.0;
		soil->K = 150.0;
		soil->pH = 6.9;
		soil->SOM = 0.25;

		auto chem = ChemicalSchemeDto::createShared();
		chem->lat = 39.23211;
		chem->lon = 109.1233432;
		chem->pygd = 10;
		chem->pyfk = 4;
		chem->pysb = "M1";
		chem->pzlx = "CENTRIFUGAL_NOZZLE";
		chem->pyfs = "POINT_SPRAY";
		chem->pykg = "ON";
		chem->rote_speed = 400;
		chem->flow_speed = 2.4;
		chem->droplet_size = 2.4;
		chem->penzui_size = 20;
		chem->point_status = 2;

		auto task = FertilizeTaskDto::createShared();
		task->id = 24151;
		task->spray_guid = "6ca2359b-0c3e-4f1d-aafd-b353e7fcc1ea";
		task->crop_type = u8"玉米";
		task->growth_stage = u8"大喇叭口";
		task->pest_type = u8"玉米螟";
		task->severity_level = u8"LEVEL1";
		task->tank_capacity = 30.0;
		task->environment = env;
		task->soil_info = soil;
		task->total_area = 7.7;
		task->chemical_scheme = { chem };
		task->total_amount = 150.0;
		task->flight_params = 1200.0;
		task->estimated_time = 15.5;
		task->max_flighttime = 30.0;

		auto response = FertilizeTaskResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = task;

		return createDtoResponse(Status::CODE_200, response);
	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // UAVFertilizeController_h__
