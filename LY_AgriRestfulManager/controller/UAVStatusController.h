#ifndef UAVStatusController_h__
#define UAVStatusController_h__

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"
#include "oatpp/json/ObjectMapper.hpp"

#include "LY_AgriRestfulManager.h"

#include "DTO/UAVStatusDTO.h"

#include "IAgriRestfulManagerActivator.h"

#include "logger/taskLogger.h"


#include OATPP_CODEGEN_BEGIN(ApiController)

class UAVStatusController : public oatpp::web::server::api::ApiController {
public:
	UAVStatusController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
		: oatpp::web::server::api::ApiController(apiContentMappers)
	{}

	ENDPOINT("GET", "/api/v1/uav/status", getUAVStatus) {

		auto start = std::chrono::steady_clock::now();

		// 查询任务状态
		UavStatus status;
		auto agriSprayPrescriptService = IAgriRestfulManagerActivator::Get().getPIAgriSprayPrescriptionService();
		if (agriSprayPrescriptService)
		{
			status = agriSprayPrescriptService->getUavStatus();
		}

		auto attitude = AttitudeDto::createShared();
		attitude->pitch = status.pitch;
		attitude->roll = status.roll;
		attitude->yaw = status.yaw;

		auto data = DroneStatusDto::createShared();
		data->comm_status = status.comm_status.toStdString();
		data->rc_status = status.rc_status.toStdString();
		data->flight_mode = status.flight_mode.toStdString();
		data->gps_status = status.gps_status.toStdString();
		data->satellite_count = status.satellite_count;
		data->stage = status.stage.toStdString();
		data->voltage = status.voltage;
		data->current = status.current;
		data->latitude = status.latitude;
		data->longitude = status.longitude;
		data->altitude = status.altitude;
		data->speed = status.speed;
		data->attitude = attitude;
		data->task_stage = status.task_stage.toStdString();

		auto response = ResponseDto::createShared();
		response->code = 0;
		response->message = "success";
		response->data = data;

		
		auto end = std::chrono::steady_clock::now();
		int durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		// 获取当前时间戳
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string timeStr = std::ctime(&now);
		timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1); // 清除换行

		// 序列化为son字符串
		auto objectMapper = oatpp::json::ObjectMapper::ObjectMapper();
		oatpp::String jsonStr = objectMapper.writeToString(data);

		RequestLog log;
		log.endpoint = "/api/v1/uav/status";
		log.method = "GET";
		log.timestamp = timeStr;
		log.responseCode = 200;
		log.durationMs = durationMs;
		log.payloadJson = jsonStr->c_str(); // 转为 std::string

		taskLoggerUtil::writeLogToFile(log); // 写入日志

		return createDtoResponse(Status::CODE_200, response);
	}
};

#include OATPP_CODEGEN_END(ApiController)
#endif // UAVStatusController_h__
