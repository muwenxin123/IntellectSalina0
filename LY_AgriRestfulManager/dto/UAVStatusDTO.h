#ifndef UAVStatusDTO_h__
#define UAVStatusDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AttitudeDto : public oatpp::DTO {
	DTO_INIT(AttitudeDto, DTO)

	DTO_FIELD(Float64, pitch);
	DTO_FIELD(Float64, roll);
	DTO_FIELD(Float64, yaw);
};

class DroneStatusDto : public oatpp::DTO {
	DTO_INIT(DroneStatusDto, DTO)

	DTO_FIELD(String, comm_status);
	DTO_FIELD(String, rc_status);
	DTO_FIELD(String, flight_mode);
	DTO_FIELD(String, gps_status);
	DTO_FIELD(Int32, satellite_count);
	DTO_FIELD(String, stage);
	DTO_FIELD(Float64, voltage);
	DTO_FIELD(Float64, current);
	DTO_FIELD(Float64, latitude);
	DTO_FIELD(Float64, longitude);
	DTO_FIELD(Float64, altitude);
	DTO_FIELD(Float64, speed);
	DTO_FIELD(Object<AttitudeDto>, attitude);
	DTO_FIELD(String, task_stage);
};

class ResponseDto : public oatpp::DTO {
	DTO_INIT(ResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<DroneStatusDto>, data);
};

#include OATPP_CODEGEN_END(DTO)
#endif // UAVStatusDTO_h__
