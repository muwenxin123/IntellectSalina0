#ifndef OELAYERCONTROL_DIALOG_H
#define OELAYERCONTROL_DIALOG_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QApplication>

#include "oeLayerControl.h"
#include "IProj.h"


namespace Ui
{
class oeLayerControl_Dialog;
}

struct oeLayerControl_DialogPrivate;
class oeLayerControl_Dialog : public QWidget
{
    Q_OBJECT

public:
    explicit oeLayerControl_Dialog(QWidget *parent = nullptr);
    ~oeLayerControl_Dialog();

	void OnCreate();
    void setMapNode(osgEarth::MapNode *_mapNode);
	void drawDynamic(QPainter &painter, IProj *pProj);

protected:
    void initTreeWidget();
    void updateTreeWidget();
	void updateParentItem(QTreeWidgetItem *item);
	bool setLayerShowState(std::string LayerName, bool isShow);

protected:
	void changeEvent(QEvent *event) override;
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::oeLayerControl_Dialog *ui;
    osgEarth::MapNode *mapNode = nullptr;
    oeLayerControl    *layerControl;
    std::vector<std::pair<std::string, bool>>		imageLayerNames;			//Ó°ÏñÍ¼²ãË³Ðò  <Ãû³Æ£¬ ÏÔÒþ>
    std::vector<std::pair<std::string, bool>>		elevationLayerNames;		//¸ß³ÌÍ¼²ãË³Ðò  <Ãû³Æ£¬ ÏÔÒþ>
	std::vector<std::pair<std::string, bool>>		modelLayerNames;			//Ä£ÐÍÍ¼²ãË³Ðò  <Ãû³Æ£¬ ÏÔÒþ>
	std::vector<std::pair<std::string, bool>>		fieldLayerNames;			//Ìï¿éÍ¼²ãË³Ðò  <Ãû³Æ£¬ ÏÔÒþ>

private:
	QPoint m_dragPosition;
	bool m_dragging = false;
	oeLayerControl_DialogPrivate* _p;
};

#endif // OELAYERCONTROL_DIALOG_H
