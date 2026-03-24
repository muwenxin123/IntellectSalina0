#ifndef AGRISATELLITEPYTHONGENERATOR_H_
#define AGRISATELLITEPYTHONGENERATOR_H_

#include <QObject>

#include <iostream>
#include <mutex>

#include "AgriSatellitePythonCallback.h"


/* C++线程安全的懒加载单例模式
** >> 线程安全（使用 std::call_once）
** >> 懒加载（首次调用才初始化）
** >> 自动释放源（std::atexit + unique_ptr）
** >> 禁止拷贝（私有拷贝构造函数）
*/
class AgriSatellitePythonGenerator : public QObject
{
	Q_OBJECT

public:
	static AgriSatellitePythonGenerator& getInstance();
	
private:
	AgriSatellitePythonGenerator(QObject *parent = nullptr);
	AgriSatellitePythonGenerator(const AgriSatellitePythonGenerator&) = delete;
	AgriSatellitePythonGenerator& operator=(const AgriSatellitePythonGenerator&) = delete;
	static void destroy();

public:
	~AgriSatellitePythonGenerator();

	std::vector<int> generate_ndvi(std::vector<int> data);

	void download_sentinel_data(std::string clip_path, std::string output_dir, PyObject* cb);

private:
	void InitialPythonEnv();
	void CleanUpPythonEnv();
	void PrintPythonError();

private:
	static std::unique_ptr<AgriSatellitePythonGenerator> instance;
	static std::once_flag initFlag;
};
#endif  // AgriSatellitePythonGenerator_H