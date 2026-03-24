#ifndef UAVPatrolDTO_h__
#define UAVPatrolDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

// 巡逻航点数据
class PatrolPointDto : public oatpp::DTO {
	DTO_INIT(PatrolPointDto, DTO)

	DTO_FIELD(Int32, No);         // 航点编号
	DTO_FIELD(Int32, HXBH);       // 航线编号
	DTO_FIELD(Float64, JD);       // 经度
	DTO_FIELD(Float64, WD);       // 纬度
	DTO_FIELD(Float64, GD);       // 高度
	DTO_FIELD(Float64, SD);       // 速度
	DTO_FIELD(Float64, XTSJ);     // 悬停时间
	DTO_FIELD(String, ZWSX);      // 转弯属性
	DTO_FIELD(String, JLD);       // 降落点
	DTO_FIELD(Int32, ZSDASJ);     // 准时到达时间
};

// 数据主体
class PatrolDataDto : public oatpp::DTO {
	DTO_INIT(PatrolDataDto, DTO)

	DTO_FIELD(List<Object<PatrolPointDto>>, points);
};

// 响应对象
class PatrolResponseDto : public oatpp::DTO {
	DTO_INIT(PatrolResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<PatrolDataDto>, data);
};

#include OATPP_CODEGEN_END(DTO)

#endif // UAVPatrolDTO_h__
