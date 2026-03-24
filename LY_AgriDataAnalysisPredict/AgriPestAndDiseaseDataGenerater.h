#ifndef AGRIPESTANDDISEASEDATAGENERATER_H_
#define AGRIPESTANDDISEASEDATAGENERATER_H_
 
#include <QObject>

#include "AgriDataAnalysisPredictDecl.h"


class AgriPestAndDiseaseDataGenerater : public QObject
{
	Q_OBJECT

public:
	explicit AgriPestAndDiseaseDataGenerater(QObject *parent = nullptr);
	~AgriPestAndDiseaseDataGenerater();

	bool generatePestDataCollection(const QString& sDatasetId, const std::vector<QPointF>& vBoundPoings, int number);

private:
	PestLevel determinePestLevel(const AgriPestCollectionPtr& pestCollect);

};
#endif  // AgriPestAndDiseaseDataGenerater_H