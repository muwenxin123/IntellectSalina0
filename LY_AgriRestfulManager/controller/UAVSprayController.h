#ifndef UAVSprayController_h__
#define UAVSprayController_h__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/UAVSprayDTO.h"

#include "task/UAVSprayTask.h"


#include OATPP_CODEGEN_BEGIN(ApiController)

namespace UAVSparyDTO {

class UAVSprayController : public oatpp::web::server::api::ApiController {
public:
	UAVSprayController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("POST", "/api/v1/uav/application/plan", postSprayTask) {

		std::shared_ptr<ITask> itask = LY_AgriRestfulManager::Get().isExistTaskProcess();
		if (itask)
		{
			auto response = SprayTaskResponseDto::createShared();
			response->code = -1;
			response->message = "error: exits running task";

			return createDtoResponse(Status::CODE_200, response);
		}

		// 添加喷药任务
		std::shared_ptr<UAVSprayTask> sprayTask = std::shared_ptr<UAVSprayTask>(new UAVSprayTask());
		LY_AgriRestfulManager::Get().addTask(sprayTask);

		auto env = EnvironmentDto::createShared();
		env->temp = 22;
		env->windSpeed = 3.0;

		auto soil = SoilInfoDto::createShared();
		soil->N = 180.0;
		soil->P = 30.0;
		soil->K = 150.0;
		soil->pH = 6.9;
		soil->SOM = 0.25;

		auto chem = ChemicalSchemeDto::createShared();
		chem->lat = 40.2650542;
		chem->lon = 116.2015567;
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

		auto task = SprayTaskDto::createShared();
		task->id = 24151;
		task->spray_guid = "6ca2359b-0c3e-4f1d-aafd-b353e7fcc1ea";
		task->crop_type = u8"玉米";
		task->growth_stage = u8"大喇叭口期";
		task->pest_type = u8"玉米螟";
		task->severity_level = u8"LEVEL1";
		task->tank_capacity = 30.0;
		task->environment = env;
		task->soil_info = soil;
		task->total_area = 13.4;
		task->chemical_scheme = { chem };
		task->total_amount = 150.0;
		task->flight_params = 1450.0;
		task->estimated_time = 15.5;

		auto response = SprayTaskResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = task;

		return createDtoResponse(Status::CODE_200, response);
	}
};

}

#include OATPP_CODEGEN_END(ApiController)
#endif // UAVSprayController_h__
