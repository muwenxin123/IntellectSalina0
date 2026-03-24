#ifndef CLOGRECORD_H
#define CLOGRECORD_H

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <string>
#include  <stdio.h>
#include  <stdlib.h>

class CLogRecord
{
public:
    CLogRecord();
    CLogRecord(const std::string& fileName, const std::string fileMode);
    virtual ~CLogRecord();

    virtual bool open();
    virtual bool open(const std::string& fileName, const std::string fileMode);
    virtual bool isopen();
    virtual bool close();
    virtual size_t write(const char *buffer, size_t elementSize) = 0;
    virtual bool flush();

    virtual void setFileName(std::string fileName);
    virtual std::string fileName();
    virtual void setFileMode(std::string fileMode);
    virtual std::string fileMode();
    virtual std::string errorStr();

    virtual bool fileExists();
    virtual bool fileExists(const std::string& fileName);

protected:
    size_t fileWrite(const char *buffer, size_t elementSize, size_t elementCount);

private:
    CLogRecord(const CLogRecord& logRecord);
    CLogRecord& operator=(const CLogRecord& _Right);

private:
    FILE *      _logStream;
    std::string _errorStr;
#ifdef WIN32
    errno_t     _errno;
#else
    error_t     _errno;
#endif
    std::string _fileName;
    std::string _fileMode;
};

class CLogRecordCVS : public CLogRecord {
public:
    CLogRecordCVS(const std::string &fileHeader, const std::string& logDir, const std::string &suffix, const std::string &fileNameSuffix = "csv");
    ~CLogRecordCVS();

    virtual bool open();
    virtual size_t write(const char *buffer, size_t elementSize);

    void setFileHeader(std::string fileHeader);
    void setLogDir(std::string logDir);

protected:

private:
    CLogRecordCVS(const CLogRecordCVS& logRecord);
    CLogRecordCVS& operator=(const CLogRecordCVS& _Right);

private:
    std::string        _fileHeader;
    std::string        _logDir;
    std::string        _suffix;
    std::string        _fileNameSuffix;
    unsigned long long _logRow;
    unsigned long long _logSize;
};

class CLogRecordCVSCMD : public CLogRecord {
public:
    CLogRecordCVSCMD(const std::string &fileHeader, const std::string &logDir, const std::string &suffix, const std::string &fileNameSuffix = "csv");
    ~CLogRecordCVSCMD();

    virtual bool open();
    virtual size_t write(const char* buffer, size_t elementSize);

    void setFileHeader(std::string fileHeader);
    void setLogDir(std::string logDir);

protected:

private:
    CLogRecordCVSCMD(const CLogRecordCVSCMD& logRecord);
    CLogRecordCVSCMD& operator=(const CLogRecordCVSCMD& _Right);

private:
    std::string        _fileHeader;
    std::string        _logDir;
    std::string        _suffix;
    std::string        _fileNameSuffix;
    unsigned long long _logRow;
    unsigned long long _logSize;
};

class CLogRecordXygsServer : public CLogRecord {
public:
    CLogRecordXygsServer(const std::string &fileHeader, const std::string &logDir, const std::string &suffix, const std::string &fileNameSuffix = "log");
    ~CLogRecordXygsServer();

    virtual bool open();
    virtual size_t write(const char* buffer, size_t elementSize);

    virtual size_t write(const char* buffer, size_t elementSize, bool newLogFile);

    void setFileHeader(std::string fileHeader);
    void setLogDir(std::string logDir);

protected:

private:
    CLogRecordXygsServer(const CLogRecordXygsServer& logRecord);
    CLogRecordXygsServer& operator=(const CLogRecordXygsServer& _Right);

private:
    std::string        _fileHeader;
    std::string        _logDir;
    std::string        _suffix;
    std::string        _fileNameSuffix;
    unsigned long long _logRow;
    unsigned long long _logSize;
};

#endif
