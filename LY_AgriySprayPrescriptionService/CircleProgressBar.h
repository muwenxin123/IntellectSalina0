#ifndef CIRCLEPROGRESSBAR_H
#define CIRCLEPROGRESSBAR_H

#include <QWidget>

class CircleProgressBar : public QWidget
{
    Q_OBJECT
public:
		// 构造函数
		explicit CircleProgressBar(QWidget *parent = nullptr);
public:
	// 设置进度
	void setProgress(int progress);

protected:
	// 重写绘制事件
	void paintEvent(QPaintEvent *event) override;

private:
	int m_progress;  // 当前进度（0-100）
};

#endif // CIRCLEPROGRESSBAR_H