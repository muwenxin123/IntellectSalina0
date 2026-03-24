#ifndef DATARESULT_H
#define DATARESULT_H

#include <QWidget>
#include <QReadLocker>
#include "gdd/wsProtocol.h"
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include "ImageDetailDialog.h"
namespace Ui {
class DataResult;
}

class ClickableImageLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ClickableImageLabel(QWidget* parent = nullptr) : QLabel(parent) {
		setCursor(Qt::PointingHandCursor); //鼠标变成手
	}

	void setOriginalImage(const QImage& img) 
	{ 
		m_originalImage = img; 
	}
	QImage getOriginalImage() const {
		return m_originalImage;
	}

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *event) override {
		emit clicked();
		QLabel::mousePressEvent(event);
	}

private:
	QImage m_originalImage;
};

class DataResult : public QWidget
{
    Q_OBJECT

public:
    explicit DataResult(QWidget *parent = nullptr);
    ~DataResult();
	void shutdown();
	bool isShuttingDown() const { return m_isShuttingDown; };
	QLabel m_imageLabel;


	void switchTo1();
	void switchTo2();
	void switchTo3();
	void switchTo4();
	void switchTo5();
	void onDetectionDataReceived();
	void showImageDialog(const QImage& image, const int& info);
	void addImage(const WS::EventData& data);
	  //void addRect(const QPainter& p);
	// 图片显示相关
	QVector<QWidget*> m_imagePages;      // 图片页面
	QVector<QGridLayout*> m_imageLayouts; // 每个页面的布局
	QVector<QScrollArea*> m_scrollAreas; // 滚动区域

private:
    Ui::DataResult *ui;
	bool m_isShuttingDown = false;
	std::vector<WS::EventData> eventdatavec;
	QTimer* m_timer;
	mutable QReadWriteLock m_readLock;
	void initTable();
	void initImageDialog(); //初始化图片弹窗

	QDialog* m_imageDialog;
	QLabel* m_imageDisplayLabel;

};

#endif // DATARESULT_H
//
//#ifndef DATARESULT_H
//#define DATARESULT_H
//
//#include <QObject>
//#include <QThread>
//#include <QMap>
//#include <QWidget>
//#include <QString>
//#include "EventRecord.h"
//class QPushButton;
//class QStackedWidget;
//class QHBoxLayout;
//
//class DataResult : public QWidget
//{
//	Q_OBJECT
//
//public:
//	explicit DataResult(QWidget *parent = nullptr);
//
//	void addPage(const QString& name, QWidget* widget);
//
//	private slots:
//	void switchPage();
//public slots:
//	void setDataResult(const WS::EventData& data);
//	void onDetectionDataReceived();
//private:
//	QHBoxLayout *m_buttonLayout;
//	QStackedWidget *m_stack;
//	QList<QPushButton*> m_buttons;
//	std::vector<WS::EventData> Paintervec;
//	mutable QReadWriteLock m_readLock;
//	QTimer* m_timer;
//};
//
//#endif // SIMPLEPAGESWITCHER_H