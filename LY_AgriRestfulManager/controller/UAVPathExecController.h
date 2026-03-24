#ifndef UAVPATHEXECCONTROLLER_h__
#define UAVPATHEXECCONTROLLER_h__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/UAVPathExecDTO.h"

#include "IAgriRestfulManagerActivator.h"


#include OATPP_CODEGEN_BEGIN(ApiController)

class UAVPathExecController : public oatpp::web::server::api::ApiController {
public:
	UAVPathExecController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("POST", "/api/v1/uav/path/execute", postPathExecute) {

		// Ö´ÐÐÈÎÎñ
		std::shared_ptr<ITask> itask = LY_AgriRestfulManager::Get().isExistTaskProcess();
		if (itask)
		{
			if (itask->isWaitRequired())
			{
				itask->setWaitRequired();

				auto response = PathExecResponseDto::createShared();
				response->code = 0;
				response->message = "success";
				response->data;

				return createDtoResponse(Status::CODE_200, response);
			}
		}

		auto response = PathExecResponseDto::createShared();
		response->code = -1;
		response->message = "error: no task to be executed";
		response->data;

		return createDtoResponse(Status::CODE_200, response);

	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // UAVPATHEXECCONTROLLER_h__
