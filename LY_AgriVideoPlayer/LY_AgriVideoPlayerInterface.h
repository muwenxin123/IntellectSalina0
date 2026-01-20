#ifndef _LY_AgriQuickSprayService_Interface_H
#define _LY_AgriQuickSprayService_Interface_H

#include <IInterface.h>

class LY_AgriVideoPlayerInterface : public IInterface
{
public:
    //inline static LY_MonitorMoudleInterface& Get(){static LY_MonitorMoudleInterface gi; return gi;};
    //写成以上形式在Windows操作系统上使用VC2008编译、运行没有问题，使用QT5.5.1自带的MinGw4.9.2编译、运行也正常。
    //Ubuntu14.04操作系统上使用gcc4.8.2编译时，不能同时加载多个LY_MonitorMoudleInterface::Get()实例对象，或者改gi的名称不重复，
    //或者改为如下形式，并且必须将LY_MonitorMoudleInterface::Get()的实现函数放到cpp文件中，对象gi的名称即可以重复，在使用QLibrary加载不同的动态库中对象gi的地址是不同的，可以达到预期目的。
    static LY_AgriVideoPlayerInterface &Get();

	/**
	* @fn  void Startup()
	* - 描述:    系统启动(内部初始化完毕调用)
	* - 输入参数: 无
	* - 输出参数: 无
	* - 返回参数: 无
	*/
	virtual void Startup();

	/**
	* @fn  void Shutdown()
	* - 描述:    系统退出(界面退出前调用)
	* - 输入参数: 无
	* - 输出参数: 无
	* - 返回参数: 无
	*/
	virtual void Shutdown();

	/**
	* @fn  void Reset()
	* - 描述:    系统初始化
	* - 输入参数: 无
	* - 输出参数: 无
	* - 返回参数: 无
	*/
	virtual void Reset();

	virtual bool OnNetRecv(int nLength, char *lpData);

	virtual bool DrawGraph(QPainter &painter, IProj *pProj);
	virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
	virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
	virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
	{
		return false;
	}
	virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
	{
		return false;
	}
};

#endif // _LY_AgriQuickSprayServicePlough_Interface_H
