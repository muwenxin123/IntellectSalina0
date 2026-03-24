#include "LogRecord.h"

#include  <time.h>
#ifdef WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

// 飞行日志的目录
#define FlightLogDir "../data/Record/Record_UAVFlightLog/"
#define MAX_LOGRECORD_FILE_SIZE            536800000   // 5120000  // 单个文件的最大值
#define MAX_LOGRECORD_FILE_ROW             1048000     // 9999999  // 最大单次记录的文件数量
#define MAX_LOGRECORD_FILE_XYGSSERVER_SIZE 536800000  // 5120000  // 单个文件的最大值
#define MAX_LOGRECORD_FILE_XYGSSERVER_ROW  1048000    // 9999999  // 最大单次记录的文件数量

CLogRecord::CLogRecord()
    : _logStream(nullptr)
    , _errorStr("")
    , _errno(-1)
    , _fileName("")
    , _fileMode("")
{
}

CLogRecord::CLogRecord(const std::string &fileName, const std::string fileMode)
    : _logStream(nullptr)
    , _errorStr("")
    , _errno(-1)
    , _fileName(fileName)
    , _fileMode(fileMode)
{
}


CLogRecord::~CLogRecord()
{
    close();
}

bool CLogRecord::open()
{
    if (_fileName.empty() || _fileMode.empty())
    {
        return false;
    }

    if (_logStream != nullptr)
    {
        fclose(_logStream);
    }

    _logStream = fopen(_fileName.c_str(), _fileMode.c_str());
    if (_errno != 0)
    {
        _errorStr = ("The file " + _fileName + " was not opened, mode is " + _fileMode + ", errorno is " + std::to_string(_errno) + ".");
        return false;
    }
    return (_logStream != nullptr);
}

bool CLogRecord::open(const std::string &fileName, const std::string fileMode)
{
    if (fileName.empty() || fileMode.empty())
    {
        return false;
    }

    if (_logStream != nullptr)
    {
        close();
    }

    _logStream = fopen(fileName.c_str(), fileMode.c_str());
    if (_logStream == nullptr)
    {
        _errorStr = ("The file " + _fileName + " was not opened, mode is " + _fileMode + ", errorno is " + std::to_string(_errno) + ".");
		_errno = -1;
        return false;
    }
    _fileName = fileName;
    _fileMode = fileMode;
	_errno = 0;
    return (_logStream != nullptr);
}

bool CLogRecord::isopen()
{
    return (_logStream != nullptr) && (_errno == 0);
}

size_t CLogRecord::fileWrite(const char *buffer, size_t elementSize, size_t elementCount)
{
    if (_logStream != nullptr && buffer != nullptr)
    {
        return fwrite(buffer, elementSize, elementCount, _logStream);
    }
    return 0;
}

bool CLogRecord::close()
{
    bool closed = false;
    if (_logStream != nullptr)
    {
        closed = !fclose(_logStream); // fclose returns 0 if the stream is successfully closed.
        _logStream = nullptr;
        _errorStr = "";
        _errno    = -1;
        _fileName = "";
        _fileMode = "";
    }
    return closed;
}

bool CLogRecord::flush()
{
    bool flushed = false;
    if (_logStream != nullptr)
    {
        flushed = !fflush(_logStream); // fclose returns 0 if the stream is successfully closed.
    }
    return flushed;
}

void CLogRecord::setFileName(std::string fileName)
{
    _fileName = fileName;
}

std::string CLogRecord::fileName()
{
    return _fileName;
}

void CLogRecord::setFileMode(std::string fileMode)
{
    _fileMode = fileMode;
}

std::string CLogRecord::fileMode()
{
    return _fileMode;
}

std::string CLogRecord::errorStr()
{
    return _errorStr;
}

bool CLogRecord::fileExists()
{
    // Check for existence.
    if (_fileName.empty())
    {
        return false;
    }

    return (access(_fileName.c_str(), 0) == 0);
}

bool CLogRecord::fileExists(const std::string &fileName)
{
    return (access(fileName.c_str(), 0) == 0);
}


// CVS file
CLogRecordCVS::CLogRecordCVS(const std::string &fileHeader, const std::string &logDir, const std::string &suffix, const std::string &fileNameSuffix)
    : _fileHeader(fileHeader)
    , _logDir(logDir)
    , _suffix(suffix)
    , _fileNameSuffix(fileNameSuffix)
    , _logRow(0)
    , _logSize(0)
{

}

CLogRecordCVS::~CLogRecordCVS()
{
    close();
}

void CLogRecordCVS::setFileHeader(std::string fileHeader)
{
    _fileHeader = fileHeader;
}

void CLogRecordCVS::setLogDir(std::string logDir)
{
    _logDir = logDir;
}

bool CLogRecordCVS::open()
{
	time_t long_time;
	time(&long_time);
	// Convert to local time.
	struct tm *pTM = localtime(&long_time);
	char  fileNamebuffer[64] = { 0 };
	sprintf(fileNamebuffer, "%04d%02d%02d.csv", (pTM->tm_year + 1900), (pTM->tm_mon + 1), (pTM->tm_mday));
	std::string fileName = _logDir + fileNamebuffer;
	bool bWriteHeader = fileExists(fileName);
	bool opened = CLogRecord::open(fileName.c_str(), "a+");
	if (!bWriteHeader && opened)
	{
		CLogRecord::fileWrite(_fileHeader.c_str(), _fileHeader.size(), 1);
		flush();
	}
	return opened;
}

size_t CLogRecordCVS::write(const char *buffer, size_t elementSize)
{
    if (buffer != nullptr)
    {
        if (_logSize > MAX_LOGRECORD_FILE_SIZE || _logRow > MAX_LOGRECORD_FILE_ROW)
        {
            close();
            open();
            _logSize = 0;
            _logRow  = 0;
        }
        size_t result =  CLogRecord::fileWrite(buffer, elementSize, 1);
        if (result)
        {
            flush();
            _logRow++;
            _logSize = _logSize + result * elementSize;// result = 1
        }

        return result;
    }
    return 0;
}

// CVSCMD file
CLogRecordCVSCMD::CLogRecordCVSCMD(const std::string &fileHeader, const std::string &logDir, const std::string &suffix, const std::string &fileNameSuffix)
    : _fileHeader(fileHeader)
    , _logDir(logDir)
    , _suffix(suffix)
    , _fileNameSuffix(fileNameSuffix)
    , _logRow(0)
    , _logSize(0)
{

}

CLogRecordCVSCMD::~CLogRecordCVSCMD()
{
    close();
}

void CLogRecordCVSCMD::setFileHeader(std::string fileHeader)
{
    _fileHeader = fileHeader;
}

void CLogRecordCVSCMD::setLogDir(std::string logDir)
{
    _logDir = logDir;
}

bool CLogRecordCVSCMD::open()
{
    for (int i = 0; i < 4; i++)
    {
        time_t long_time;
        time(&long_time);
        // Convert to local time.
        struct tm *pTM = localtime(&long_time);
        char  fileNamebuffer[64] = { 0 };
        sprintf(fileNamebuffer, "%04d%02d%02d%02d%02d%02d_%03d_%s.csv", (pTM->tm_year + 1900), (pTM->tm_mon + 1), (pTM->tm_mday), (pTM->tm_hour), (pTM->tm_min), (pTM->tm_sec), i, _suffix.c_str());
        std::string fileName = _logDir + fileNamebuffer;
        if (!fileExists(fileName))
        {
            bool opened = CLogRecord::open(fileName.c_str(), "a+");
            if (opened)
            {
                CLogRecord::fileWrite(_fileHeader.c_str(), _fileHeader.size(), 1);
                flush();
                return true;
            }
        }
    }
    return false;
}

size_t CLogRecordCVSCMD::write(const char *buffer, size_t elementSize)
{
    if (buffer != nullptr)
    {
        if (_logSize > MAX_LOGRECORD_FILE_SIZE || _logRow > MAX_LOGRECORD_FILE_ROW)
        {
            close();
            open();
            _logSize = 0;
            _logRow  = 0;
        }
        size_t result = CLogRecord::fileWrite(buffer, elementSize, 1);
        if (result)
        {
            flush();
            _logRow++;
            _logSize = _logSize + result * elementSize; // result = 1
        }

        return result;
    }
    return 0;
}

// Server file
CLogRecordXygsServer::CLogRecordXygsServer(const std::string &fileHeader, const std::string &logDir, const std::string &suffix, const std::string &fileNameSuffix)
    : _fileHeader(fileHeader)
    , _logDir(logDir)
    , _suffix(suffix)
    , _fileNameSuffix(fileNameSuffix)
    , _logRow(0)
    , _logSize(0)
{

}

CLogRecordXygsServer::~CLogRecordXygsServer()
{
    close();
}

void CLogRecordXygsServer::setFileHeader(std::string fileHeader)
{
    _fileHeader = fileHeader;
}

void CLogRecordXygsServer::setLogDir(std::string logDir)
{
    _logDir = logDir;
}

bool CLogRecordXygsServer::open()
{
    for (int i = 0; i < 4; i++)
    {
        time_t long_time;
        time(&long_time);
        // Convert to local time.
        struct tm *pTM = localtime(&long_time);
        char  fileNamebuffer[64] = { 0 };
        sprintf(fileNamebuffer, "%04d%02d%02d%02d%02d%02d_%03d_%s.%s", (pTM->tm_year + 1900), (pTM->tm_mon + 1), (pTM->tm_mday), (pTM->tm_hour), (pTM->tm_min), (pTM->tm_sec), i, _suffix.c_str(), _fileNameSuffix.c_str());
        std::string fileName = _logDir + fileNamebuffer;
        if (!fileExists(fileName))
        {
            bool opened = CLogRecord::open(fileName.c_str(), "ab+");
            if (opened)
            {
                CLogRecord::fileWrite(_fileHeader.c_str(), _fileHeader.size(), 1);
                flush();
                return true;
            }
        }
    }
    return false;
}

size_t CLogRecordXygsServer::write(const char *buffer, size_t elementSize)
{
    if (buffer != nullptr)
    {
        if (_logSize > MAX_LOGRECORD_FILE_XYGSSERVER_SIZE || _logRow > MAX_LOGRECORD_FILE_XYGSSERVER_ROW)
        {
            close();
            open();
            _logSize = 0;
            _logRow  = 0;
        }
        size_t result = CLogRecord::fileWrite(buffer, elementSize, 1);
        if (result)
        {
            flush();
            _logRow++;
            _logSize = _logSize + result * elementSize; // result = 1
        }

        return result;
    }
    return 0;
}

size_t CLogRecordXygsServer::write(const char *buffer, size_t elementSize, bool newLogFile)
{
    if (buffer != nullptr)
    {
        if (newLogFile && (_logSize > MAX_LOGRECORD_FILE_XYGSSERVER_SIZE || _logRow > MAX_LOGRECORD_FILE_XYGSSERVER_ROW))
        {
            close();
            open();
            _logSize = 0;
            _logRow  = 0;
        }
        size_t result = CLogRecord::fileWrite(buffer, elementSize, 1);
        if (result)
        {
            flush();
            _logRow++;
            _logSize = _logSize + result * elementSize; // result = 1
        }

        return result;
    }
    return 0;
}
