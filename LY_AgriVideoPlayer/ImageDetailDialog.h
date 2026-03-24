#ifndef IMAGEDETAILDIALOG_H
#define IMAGEDETAILDIALOG_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QSplitter>
#include "gdd/wsProtocol.h"
#include <QCString.h>

struct DetailItem {
	QString category;      // 类别（从label解析）
	int count;             // 数量
	float avgConfidence;   // 平均置信度
	QVector<int> trackIds;  // 跟踪ID列表
};

class ImageDetailDialog : public QDialog
{
	Q_OBJECT
public:
	explicit ImageDetailDialog(QWidget *parent = nullptr);
	~ImageDetailDialog();

	void setImage(const QImage& image);
	void setEventData(const WS::EventData& eventData);  // 使用 EventData
	static QString parseClassName(const std::string& label);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	void setupUI();
	void updateDetailTable();
	void calculateDetails();

private:
	QSplitter* m_mainSplitter;
	QLabel* m_imageLabel;
	QTableWidget* m_detailTable;
	QLabel* m_infoLabel;
	QPushButton* m_exportBtn;
	QPushButton* m_annotateBtn;
	QPushButton* m_closeBtn;

	QImage m_currentImage;
	QVector<WS::BBox> m_bboxes;  // 存储检测框
	QVector<DetailItem> m_details;
	int m_totalBoxes;
	QString m_videoId;
	QTime m_captureTime;
};

#endif // IMAGEDETAILDIALOG_H