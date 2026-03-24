#ifndef LY_SHOWVIDEODATA_H
#define LY_SHOWVIDEODATA_H

#include <QWidget>
#include <QGuiApplication>
#include <QScreen>

namespace Ui {
class LY_ShowVideoData;
}

class LY_ShowVideoData : public QWidget
{
    Q_OBJECT

public:
    explicit LY_ShowVideoData(QWidget *parent = nullptr);
    ~LY_ShowVideoData();


	//设置窗口显示位置
	void setWidgetPosition();
	//设置窗口中控件样式
	void SetStyleSheetWidget();
protected:
	// 重写事件过滤器
	bool eventFilter(QObject *obj, QEvent *event) override;

public:
public:
private:
    Ui::LY_ShowVideoData *ui;
};

#endif // LY_SHOWVIDEODATA_H
