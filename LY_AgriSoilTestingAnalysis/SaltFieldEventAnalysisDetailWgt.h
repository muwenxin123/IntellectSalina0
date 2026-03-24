#ifndef SALT_FIELD_EVENT_ANALYSIS_DETAIL_WGT_H__
#define SALT_FIELD_EVENT_ANALYSIS_DETAIL_WGT_H__

#include <QWidget>

#include "AgriSoilAnalysisDBHandler.h"

namespace Ui
{
	class SaltFieldEventAnalysisDetailWgt;
}

class SaltFieldEventAnalysisDetailWgt : public QWidget {
	Q_OBJECT
public:
	explicit SaltFieldEventAnalysisDetailWgt(QWidget *parent = nullptr);
	~SaltFieldEventAnalysisDetailWgt();

	void setData(const QString& saltFieldName, SaltFieldEventInfoPtr soilTestingDataset);

	QString GetWeaponName(const QString& reporterID);

private:
	Ui::SaltFieldEventAnalysisDetailWgt* ui;
	QMap<int, SaltFieldEventTypeInfo>										m_eventID2DataMap;
};

#endif // SALT_FIELD_EVENT_ANALYSIS_DETAIL_WGT_H__
