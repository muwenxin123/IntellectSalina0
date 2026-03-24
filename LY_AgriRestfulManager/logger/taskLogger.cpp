#include "taskLogger.h"

void taskLoggerUtil::writeLogToFile(const RequestLog& log, const std::string& filename /*= "api_log.json"*/)
{
	std::ofstream file(filename, std::ios::app); // ×·¼ÓÐ´Èë
	if (file.is_open()) {
		file << log.toJson().dump() << std::endl;
		file.close();
	}
}
