#ifndef AGRISATELLITEDATADIALOG_H_
#define AGRISATELLITEDATADIALOG_H_

#include <QTableWidget>

#include "LyDialog.h"

namespace Ui {
	class AgriSatelliteDataDialog;
}

class AgriSatelliteDataDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriSatelliteDataDialog(QWidget *parent = nullptr);
	~AgriSatelliteDataDialog();

	void Init();

private:
	Ui::AgriSatelliteDataDialog		*ui;
	//AgriSatellitePythonGenerator	* m_pGenerator = nullptr;
};
#endif  // AgriSatelliteDataDialog_H