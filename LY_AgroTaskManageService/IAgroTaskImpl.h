#ifndef IAgroTaskImpl_H
#define IAgroTaskImpl_H

#include "DataManage/IAgroTaskService.h"
#include "DataManage/Zone/AgroTaskInfo.h"
#include "QPainter"
#include "QString"
#include <vector>

namespace qnzkna
{
namespace AgroTask
{
class IAgroTaskImpl:
    public IAgroTaskService
{
public:

    IAgroTaskImpl(void);

    ~IAgroTaskImpl(void);

	void DrawDynamic(QPainter *painter);

	//读取数据库
	void loadDataFromDB_CAgroRefDataInfoVec();
	void loadDataFromDB_CFarmlandAttributeInfoVec();
	void loadDataFromDB_CFarmEvevtInfoVec();


	//获取农业参考数据表
	CAgroRefDataInfoVec getAgroRefDataInfoVec();
	CAgroRefDataInfoVec getAgroRefDataInfo(const QString &strSeedNo);
	
	//盐田属性表添加数据
	bool addFarmlandAttributeInfo(FarmlandAttributeInfo info);

	//盐田属性表删除数据
	bool deleteFarmlandAttributeInfo(int no);

	//获取盐田属性表
	CFarmlandAttributeInfoVec getFarmlandAttributeInfoVec();
	FarmlandAttributeInfo getFarmlandAttributeInfo(int no);
	
	//获取盐田事件表
	CFarmEvevtInfoVec getFarmEvevtInfoVec();
	FarmEvevtInfo getFarmEvevtInfo(int no);

	//种子状态对照
	QString getSeedStateRefer(int seedState, int seedSubState = -1);

private:
	void initSeedStateVec();

private:
	CAgroRefDataInfoVec m_AgroRefDataInfoVec;
	CFarmlandAttributeInfoVec m_FarmlandAttributeInfoVec;
	CFarmEvevtInfoVec m_FarmEvevtInfoVec;

	std::vector<QString> seedStateVec;
	std::vector<std::vector<QString>> seedSubStateVec;

};
}
}

#endif // IAgroTaskImpl_H
