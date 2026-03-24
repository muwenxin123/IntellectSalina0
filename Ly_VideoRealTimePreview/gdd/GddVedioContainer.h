#ifndef GDDVIDEOCONTAINER_H
#define GDDVIDEOCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QList>

#include "wsProtocol.h"  // 包含结构体定义
namespace Ui {
class GddVedioContainer;
}

class GddVedioContainer : public QWidget
{
    Q_OBJECT
public:	
    explicit GddVedioContainer(QWidget *parent = nullptr);
    ~GddVedioContainer();

	void setData(WS::DetectionData& data);

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	void paintRect(QPainter& p);


private:
    Ui::GddVedioContainer *ui;
	WS::DetectionData m_data;
};

#endif // GddVedioContainer_H
