#ifndef GDDVIDEOCONTAINER_H
#define GDDVIDEOCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QList>

#include "wsProtocol.h"  // 包含结构体定义
#include "LyDialog.h"
namespace Ui {
class GddVedioContainer;
}

class GddVedioContainer : public LyDialog
{
    Q_OBJECT
public:	
    explicit GddVedioContainer(QWidget *parent = nullptr);
    ~GddVedioContainer();

	void setData(const WS::DetectionData& data);

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	void paintRect(QPainter& p);


public:
    Ui::GddVedioContainer *ui;
	WS::DetectionData m_data;
};

#endif // GddVedioContainer_H
