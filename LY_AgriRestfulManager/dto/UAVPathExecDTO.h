#ifndef UAVPATHEXECDTO_h__
#define UAVPATHEXECDTO_h__

#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class PathExecDto : public oatpp::DTO {
	DTO_INIT(PathExecDto, DTO)
};

class PathExecResponseDto : public oatpp::DTO {
	DTO_INIT(PathExecResponseDto, DTO)

	DTO_FIELD(Int32, code);
	DTO_FIELD(String, message);
	DTO_FIELD(Object<PathExecDto>, data);
};

#include OATPP_CODEGEN_END(DTO)
#endif // UAVPATHEXECDTO_h__
