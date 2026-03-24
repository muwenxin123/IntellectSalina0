#ifndef LY_AGRISPRAYTREEWIDGET_H
#define LY_AGRISPRAYTREEWIDGET_H

#include <QWidget>
#include "LY_AgriSprayPrescriptionActivator.h"
#include <QTreeWidget>
#include "ly_sprayprescriptionuntil.h"
#include "IProj.h"
#include "map3d/IEarth.h"
#include "ly_agrispraytreewidget.h"

namespace Ui {
class LY_AgriSprayTreeWidget;
}

class LY_AgriSprayTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriSprayTreeWidget(QWidget *parent = nullptr);
    ~LY_AgriSprayTreeWidget();

    void switchActiveFarm();
    void zoneHighLight(QPainter &painter, IProj *pProj);
	void clearZoneHighLight();
	void init();

private:
      QString m_currentFarm;
	  AgriFieldInfoVec										m_vecFieldList;

      ThreadSafeMap<qnzkna::ZoneManage::CZoneBase>			m_zoneDrawMap;

private slots:

      void onItemClicked(QTreeWidgetItem *item, int column);

signals:
    void leafNodeClicked(QTreeWidgetItem *item, int column);

	void setupField(QString &text, QString &file_id);

private:
    
    void selectedActiveFarm();
    void addTree(QTreeWidget *treeWidget);
    void setScreenCenter(double &dLon, double &dLat);

private:
    Ui::LY_AgriSprayTreeWidget *ui;
};

#endif // LY_AGRISPRAYTREEWIDGET_H
