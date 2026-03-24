#ifndef LY_PESTMOINTORWIDGET_H
#define LY_PESTMOINTORWIDGET_H
#include "ly_mointorviewmodel.h"
#include <QWidget>

namespace Ui {
class LY_PestMointorWidget;
}

class LY_PestMointorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_PestMointorWidget(QWidget *parent = nullptr);
    ~LY_PestMointorWidget();


private:

	void addSlots();
	void setupUI();

private slots:

	void getPestData(PestMointer p);
	void reciveDeviceStatus(PEST_DEVICE_STATUS type, bool isOpen);
	void errorCurrent();
	void getWormOperationLog(QVector <WormOperationLog> vec);
	void getHistoryData(WormHistToryData data);


private:
	bool m_isWormFlap = false; // 杀虫挡板 0 关 1 开
	bool m_isShake = false; // 震动装置 0 关 1 开
	bool m_isDryingFlap = false; // 烘干挡板 0 关 1 开
	bool m_isInsecticide = false; // 杀虫控制 0 关 1 开
	bool m_isMoveWorm = false; // 移虫挡板 0 关 1 开
	bool m_isDrying = false; // 烘干控制 0 关 1 开
	bool m_isRainFlap = false; // 雨虫挡板 0 关 1 开
	bool m_isAttractWorm = false; // 诱虫灯开关 0 关 1 开
	bool m_isFillLight = false; //补光灯 0 关 1开
	bool m_camera = false; //摄像头 0 关 1开

private:
    Ui::LY_PestMointorWidget *ui;


};

#endif // LY_PESTMOINTORWIDGET_H
