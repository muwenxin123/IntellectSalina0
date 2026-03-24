#include "curlutil.h"

#include "CurlUtil.h"

#define CURL_STATICLIB  // libcurl需要
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Ws2_32.lib")  // libcurl需要
#pragma comment(lib, "Wldap32.lib")  // libcurl需要
#pragma comment(lib, "Crypt32.lib")  // libcurl需要
#pragma comment(lib, "Normaliz.lib")  // libcurl需要

#ifdef _DEBUG
#pragma comment(lib, "../../lib/libcurl/lib/libcurl_a_debug.lib")
#else
#pragma comment(lib, "../../lib/libcurl/lib/libcurl_a.lib")
#endif


CurlUtil::CurlUtil()
{
}

// curl 读取到的数据保存到 std::string
 size_t CurlUtil::curlSaveResponseToStdString(void* contents, size_t size, size_t nmemb, std::string* s) {
	size_t newLength = size * nmemb;
	size_t oldLength = s->size();
	s->resize(oldLength + newLength);
	std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);

	return size * nmemb;
}

/**
* @brief 执行 HTTP GET 请求
* @param url  请求的 URL
* @param code 请求返回的状态码的指针
* @param headers 请求头
* @return 请求执行成功时返回响应的字符串，失败则返回空字符串，请求是否执行成功可以通过 code 进行判断
*/
string CurlUtil::get(const char* url, CURLcode* code, std::list<const char*> headers) {
	string response;

	// 初始化 curl
	CURL* curl = curl_easy_init();

	if (curl) {
		struct curl_slist* tempHeaders = NULL;

		std::list<const char*>::const_iterator iter;
		for (iter = headers.cbegin(); iter != headers.cend(); ++iter) {
			tempHeaders = curl_slist_append(tempHeaders, *iter);
		}
		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, tempHeaders);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlSaveResponseToStdString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);

		CURLcode temp = curl_easy_perform(curl);

		if (code != NULL) {
			*code = temp;
		}

		curl_slist_free_all(tempHeaders); /* free the header list */
	}
	else {
		if (code != NULL) {
			*code = CURLE_FAILED_INIT;
		}
	}

	// 释放 curl 资源
	curl_easy_cleanup(curl);

	return response;
}

/**
* @brief 执行 HTTP POST 请求
* @param url  请求的 URL
* @param data 请求的参数
* @param jsonBody 如果为 true，则请求的参数是 JSON 格式，否则为 Form 表单的格式 key1=value1&key2=value2&...
* @param code 请求返回的状态码的指针
* @param headers 请求头
* @return 请求执行成功时返回响应的字符串，失败则返回空字符串，请求是否执行成功可以通过 code 进行判断
*/
string CurlUtil::post(const char* url, const char* data, bool jsonBody, CURLcode* code, std::list<const char*> headers) {
	string response;

	// 初始化 curl
	CURL* curl = curl_easy_init();

	if (curl) {
		// Headers
		struct curl_slist* tempHeaders = NULL;

		std::list<const char*>::const_iterator iter;
		for (iter = headers.cbegin(); iter != headers.cend(); ++iter) {
			tempHeaders = curl_slist_append(tempHeaders, *iter);
		}

		if (jsonBody) {
			tempHeaders = curl_slist_append(tempHeaders, "Accept: application/json; charset=utf-8");
			tempHeaders = curl_slist_append(tempHeaders, "Content-Type: application/json");
		}

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, tempHeaders);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlSaveResponseToStdString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curl, CURLOPT_POST, 1); // POST 请求
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); // POST 参数

		CURLcode temp = curl_easy_perform(curl);

		if (code != NULL) {
			*code = temp;
		}

		curl_slist_free_all(tempHeaders); /* free the header list */
	}
	else {
		if (code != NULL) {
			*code = CURLE_FAILED_INIT;
		}
	}

	// 释放 curl 资源
	curl_easy_cleanup(curl);

	return response;
}