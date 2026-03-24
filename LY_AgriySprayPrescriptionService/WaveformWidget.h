#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QPainterPath>

class WaveformWidget : public QWidget
{
    Q_OBJECT
public:
	// 构造函数
	explicit WaveformWidget(QWidget *parent = nullptr);

	// 设置波形数据（可选，也可以直接内置固定数据演示）
	void setWaveformData(const QVector<qreal> &data);
	// 生成波形数据（演示用）
	void generateWaveform();        

protected:
	// 重写绘制事件
	void paintEvent(QPaintEvent *event) override;

private:
	QVector<qreal> m_waveformData;  // 波形数据
	QPainterPath m_wavePath;        // 波形路径
	QColor m_fillColor;             // 填充颜色
	
};

#endif // WAVEFORMWIDGET_H