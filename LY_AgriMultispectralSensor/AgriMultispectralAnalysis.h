#ifndef AGRIMULTISPECTRALANALYSIS_H_
#define AGRIMULTISPECTRALANALYSIS_H_
 
#include <QWidget>

#include "AgriSatelliteDataDialog.h"
#include "AgriFieldIdentifyDialog.h"
#include "AgriDataAnalysisPredictDialog.h"

namespace Ui
{
	class AgriMultispectralAnalysis;
}

struct AgriMultispectralAnalysisPrivate;
class AgriMultispectralAnalysis : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMultispectralAnalysis(QWidget *parent = nullptr);
	~AgriMultispectralAnalysis();

	AgriFieldIdentifyDialog* getFieldIdentifyDialog();

private:
	Ui::AgriMultispectralAnalysis					*ui;
	AgriSatelliteDataDialog							*m_satelliteDataDialog = nullptr;
	AgriMultispectralAnalysisPrivate*				_p;
	AgriFieldIdentifyDialog							*m_pAgriFieldIdentifyDialog = nullptr;
	AgriDataAnalysisPredictDialog					*m_pAgriDataAnalysisPredictDialog = nullptr;
};
#endif  // AGRIMULTISPECTRALANALYSIS_H