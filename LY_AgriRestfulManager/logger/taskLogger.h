#ifndef TASKLOGGER_h__
#define TASKLOGGER_h__

#include <oatpp/Types.hpp>
#include <nlohmann/json.hpp> // 使用 nlohmann/json 库
#include <fstream>
#include <chrono>


struct RequestLog {
	std::string endpoint;
	std::string method;
	std::string timestamp;
	int responseCode;
	int durationMs;
	std::string payloadJson; // 用于记录 JSON 字符串

	nlohmann::json toJson() const {
		nlohmann::json j;
		j["endpoint"] = endpoint;
		j["method"] = method;
		j["timestamp"] = timestamp;
		j["responseCode"] = responseCode;
		j["durationMs"] = durationMs;
		j["payload"] = nlohmann::json::parse(payloadJson); // 反序列化为嵌套 JSON
		return j;
	}
};


class taskLoggerUtil {

public:
	static void writeLogToFile(const RequestLog& log, const std::string& filename = "api_log.json");
};

#endif // TASKLOGGER_h__
