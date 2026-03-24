#ifndef AGRIMULTISPECTRALSENSORWIDGET_H_
#define AGRIMULTISPECTRALSENSORWIDGET_H_
 
#include <QWidget>

#include "IProj.h"
#include "AgriMultispectralSensorDecl.h"

namespace Ui
{
	class AgriMultispectralSensorWidget;
}

class AgriPestAndDiseaseDataSetDialog;
class AgriMultispectralSensorWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AgriMultispectralSensorWidget(QWidget *parent = nullptr);
	~AgriMultispectralSensorWidget();

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void Init();

private:
	void HandleMSIStatus(const AgriMSICameraStatusInfoPtr & status);

private slots:
	void on_timeout_status();

private:
	Ui::AgriMultispectralSensorWidget							*ui;
	bool														m_show = false;
	QTimer														*m_statusTimer = nullptr;
};
#endif  // AgriMultispectralSensorWidget_H