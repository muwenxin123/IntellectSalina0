#include "CommFunc.h"
#include <QCString.h>
#include "LY_AgriSoilAnalysisPredict.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

GetAreaIDFunc::GetAreaIDFunc(void)
{
}

GetAreaIDFunc::~GetAreaIDFunc(void)
{
}

QVariant GetAreaIDFunc::Handle(const QVariant &value)
{
	Q_UNUSED(value);
	QString identity;
	if (value.canConvert<QPointF>())
	{
		auto pt = value.toPointF();
		identity = LY_AgriSoilAnalysisPredict::Get().GetAreaID(pt.x(), pt.y());
	}
	return identity;
}

AddSaltFieldEventFunc::AddSaltFieldEventFunc(void)
{

}

AddSaltFieldEventFunc::~AddSaltFieldEventFunc(void)
{

}

QVariant AddSaltFieldEventFunc::Handle(const QVariant &value)
{
	bool isOk = false;
	if (value.canConvert<SaltFieldEventInfo>())
	{
		SaltFieldEventInfo info = value.value<SaltFieldEventInfo>();
		isOk =	LY_AgriSoilAnalysisPredict::Get().AddEvent(info);
	}
	return isOk;
}
