#ifndef CURLUTIL_H
#define CURLUTIL_H

#include <QObject>
#include <string>
#include <iostream>
#include <list>
#include <curl/curl.h>


using namespace std;

class CurlUtil 
{

public:

	static CurlUtil& Get() {
		static CurlUtil m_curlUtil;
		return m_curlUtil;
	}

	// 执行 HTTP GET 请求
	string get(const char* url, CURLcode* code = NULL, std::list<const char*> headers = std::list<const char*>());
	// 执行 HTTP POST 请求
	string post(const char* url, const char* data = NULL, bool jsonBody = false, CURLcode* code = NULL, std::list<const char*> headers = std::list<const char*>());

private:
	CurlUtil();

	// curl 读取到的数据保存到 std::string
	static size_t curlSaveResponseToStdString(void* contents, size_t size, size_t nmemb, std::string* s);

public slots:
};

#endif // CURLUTIL_H