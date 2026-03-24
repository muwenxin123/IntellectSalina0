#ifndef LY_HARMMANAGERWIDGET_H
#define LY_HARMMANAGERWIDGET_H

#include <QWidget>
#include <QPainter>
#include "ly_mointorviewmodel.h"
#include "ly_cameralayoutwidget.h"
#include "ly_photolayoutwidget.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include <set>
using namespace std;
namespace Ui {
class LY_HarmManagerWidget;
}

class LY_HarmManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_HarmManagerWidget(QWidget *parent = nullptr);
    ~LY_HarmManagerWidget();


protected:

	virtual void paintEvent(QPaintEvent *event);
	virtual void showEvent(QShowEvent *event);
	//virtual void hideEvent(QShowEvent *event);

public slots :

void clickPtotoBtn();
void on_qbComboBoxChanged(const int &);
void on_qblyComboBoxChanged(const int &);
void on_histyComboBoxChanged(const int &);
void on_nt_comboBox_Changed(const int &index);
void on_sbxx_comboBox_Changed(const int &index);


void receiveOrignalImage(QImage image);
void receiveRecognitionlImage(QImage image);

signals:

void sendOrignalImage(QImage image);
void sendRecognitionlImage(QImage image);
private:


	void init();

	void setItem(PestPhotoInficationVec vec);
	QByteArray original_byteArray;
	QImage       recognize_byteArray;
	PestPhotoInficationVec m_pestList;


	LY_CameraLayoutWidget *cameraLayoutWIdget;
	LY_PhotoLayoutWidget *photoLayoutWidget;

	vector<QString> timeVec;
	//AgriFieldInfoModelVec m_vecFieldList;

private:
    Ui::LY_HarmManagerWidget *ui;

	SensorListModelVec filterDuplicateMointIDs(const SensorListModelVec& sensors) {
		std::set<QString> seenMointIDs;
		SensorListModelVec result;

		for (const auto& sensor : sensors) {
			if (seenMointIDs.insert(sensor.mointID).second) {
				result.push_back(sensor);
			}
		}

		return result;
	}
};

#endif // LY_HARMMANAGERWIDGET_H
