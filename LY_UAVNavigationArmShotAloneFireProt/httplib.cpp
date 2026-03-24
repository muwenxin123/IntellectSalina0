#include <iostream>
#include <string>
#include "PlayVideoManage/httplib.h"  // 需要下载 httplib.h 头文件

int main() {
	// 创建 HTTP 客户端
	httplib::Client cli("http://192.168.2.107:9090");

	// 发送 GET 请求
	auto res = cli.Get("/get");

	if (res && res->status == 200) {
		std::cout << "GET 请求成功!" << std::endl;
		std::cout << "状态码: " << res->status << std::endl;
		std::cout << "响应体: " << res->body << std::endl;
	}
	else {
		std::cout << "GET 请求失败!" << std::endl;
		if (res) {
			std::cout << "状态码: " << res->status << std::endl;
		}
	}

	return 0;
}