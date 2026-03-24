#ifndef UAVPatrolController_h__
#define UAVPatrolController_h__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/UAVPatrolDTO.h"

#include "task/UAVPatrolTask.h"

#include OATPP_CODEGEN_BEGIN(ApiController)

class UAVPatrolController : public oatpp::web::server::api::ApiController {
public:
	UAVPatrolController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("POST", "/api/v1/uav/patrol/plan", postPatrolTask) {

		std::shared_ptr<ITask> itask = LY_AgriRestfulManager::Get().isExistTaskProcess();
		if (!itask)
		{
			// Ìí¼ÓÑ²ÂßÈÎÎñ
			std::shared_ptr<UAVPatrolTask> patrolTask = std::shared_ptr<UAVPatrolTask>(new UAVPatrolTask());
			LY_AgriRestfulManager::Get().addTask(patrolTask);
		}

		auto point = PatrolPointDto::createShared();
		point->No = 11057;
		point->HXBH = 101;
		point->JD = 116.1975119;
		point->WD = 40.2654237;
		point->GD = 10.0;
		point->SD = 2.0;
		point->XTSJ = 0.0;
		point->ZWSX = "1";
		point->JLD = "0";
		point->ZSDASJ = 0;

		auto data = PatrolDataDto::createShared();
		data->points = { point };

		auto response = PatrolResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = data;

		return createDtoResponse(Status::CODE_200, response);
	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // UAVPatrolController_h__
