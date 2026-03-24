#include <iostream>
#include <mutex>
#include <memory>
#include <thread>
#include <windows.h>

#include <QDebug>
#include "QCString.h"

#include "AgriSatellitePythonGenerator.h"

static PyThreadState *m_mainThreadState = nullptr;

// 静态成员定义
std::unique_ptr<AgriSatellitePythonGenerator> AgriSatellitePythonGenerator::instance = nullptr;
std::once_flag AgriSatellitePythonGenerator::initFlag;

AgriSatellitePythonGenerator& AgriSatellitePythonGenerator::getInstance()
{
	std::call_once(initFlag, []() {
		instance.reset(new AgriSatellitePythonGenerator());
		std::atexit(&AgriSatellitePythonGenerator::destroy);  // 注册程序结束时清理资源
	});
	return *instance;
}

AgriSatellitePythonGenerator::AgriSatellitePythonGenerator(QObject *parent /*= nullptr*/)
{
	InitialPythonEnv();

	PyEval_InitThreads();  // 初始化全局解释器锁 GIL（Python 3.7 不可省）

	m_mainThreadState = PyEval_SaveThread();  // 允许其他线程获取 GIL
}

AgriSatellitePythonGenerator::~AgriSatellitePythonGenerator()
{
	/* 注意：使用 detach() 启动线程，而主线程很快执行了 Py_Finalize()，
		解释器会关闭，后台线程仍在使用 Python 对象，会造成崩溃（Segfault）。*/

	qDebug() << tr2("Python 解释器会关闭");

	PyEval_RestoreThread(m_mainThreadState);  // 恢复主线程状态

	CleanUpPythonEnv();
}

void AgriSatellitePythonGenerator::destroy()
{
	instance.reset();  // 自动释放资源
}

std::vector<int> AgriSatellitePythonGenerator::generate_ndvi(std::vector<int> data)
{
	// 在这里导入你的 .pyd 文件，注意一定不要加后缀，比如testForCpp2Py.cp310-win_amd64.pyd就写testForCpp2Py就行
	PyObject* pModule = PyImport_ImportModule("PyMultispectral");

	// 声明一个用于接收结果的数组
	std::vector<int> result{};

	// 下面实现对.pyd文件中的函数的操作（前提是前面的操作没问题，pModule不会是空指针
	if (pModule != nullptr) {
		PyObject* pFunc = PyObject_GetAttrString(pModule, "generate_ndvi"); // 获取函数，这里填写要调用的函数名，笔者这里是test_cpp2py
		if (pFunc && PyCallable_Check(pFunc)) { // 判断该函数是否存在且可调用

			// 准备参数（两个字符串）
			const char* clip_path = "F:/wuchunrui/Others/ZS/data/shp/plot_84.shp";
			const char* output_dir = "D:/sentinel_ndvi/output6";

			// 构建参数元组
			PyObject* pArgs = PyTuple_New(2);
			PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(clip_path));
			PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(output_dir));

			// 调用函数
			PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
			// 处理返回值
			if (pResult != nullptr) {
				Py_DECREF(pResult);
			}
			Py_DECREF(pArgs);
			Py_DECREF(pFunc);
		}
		Py_DECREF(pModule);
	}
	else {
		PrintPythonError(); // 打印错误信息
	}

	return result; // 返回结果
}

void AgriSatellitePythonGenerator::download_sentinel_data(std::string clip_path, std::string output_dir, PyObject* cb)
{
	//std::thread worker([=]() {

		// 每个线程都需要调用这个获取 GIL
		PyGILState_STATE gstate = PyGILState_Ensure();

		PyObject* pModule = PyImport_ImportModule("PyMultispectral");

		if (pModule != nullptr) {
			PyObject* pFunc = PyObject_GetAttrString(pModule, "download_data");
			if (pFunc && PyCallable_Check(pFunc)) {

				PyObject* pArgs = PyTuple_New(3);

				const char * t_clip_path = "F:/wuchunrui/Others/ZS/data/shp/plot_84.shp";
				const char * t_save_path = output_dir.c_str();

				PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(t_clip_path));
				PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(t_save_path));
				PyTuple_SetItem(pArgs, 2, cb);

				PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
				if (pResult != nullptr) {
					Py_DECREF(pResult);
				}
				Py_DECREF(pArgs);
				Py_DECREF(pFunc);
			}
			Py_DECREF(pModule);
		}
		else {
			PrintPythonError(); // 打印错误信息
		}

		// 释放 GIL
		PyGILState_Release(gstate);
	//});

	//worker.detach();
}

void AgriSatellitePythonGenerator::InitialPythonEnv()
{
	// 初始化 Python 解释器
	Py_Initialize();

	// 添加 .pyd 文件所在目录到 Python 路径
	PyObject* sysPath = PySys_GetObject("path");
	// 下面是为导入.pyd文件做准备
	PyObject* pPath = PyUnicode_FromString("F:/wuchunrui/Others/pyndvi"); // 这里改成.pyd文件所在目录的路径，原理类似于环境变量
	// 下面是导入python脚本引入的第三方包
	PyObject* pPath_packages = PyUnicode_FromString("E:/Workspace/testPy/.venv/Lib/site-packages"); // 将路径改为你调用的python脚本的解释器的 Lib/site-packages 目录
	// 将上述路径加入python系统路径
	PyList_Append(sysPath, pPath);
	Py_DECREF(pPath);
	PyList_Append(sysPath, pPath_packages);
	Py_DECREF(pPath_packages);
}

void AgriSatellitePythonGenerator::CleanUpPythonEnv()
{
	// 关闭 Python 解释器
	Py_Finalize();
}

void AgriSatellitePythonGenerator::PrintPythonError()
{
	if (PyErr_Occurred()) {
		PyErr_Print();

		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

		if (pvalue) {
			PyObject* pStr = PyObject_Str(pvalue);
			const char* errStr = PyUnicode_AsUTF8(pStr);
			OutputDebugStringA(errStr);
			OutputDebugStringA("\n");
			Py_XDECREF(pStr);
		}
		Py_XDECREF(ptype);
		Py_XDECREF(pvalue);
		Py_XDECREF(ptraceback);
	}
}
