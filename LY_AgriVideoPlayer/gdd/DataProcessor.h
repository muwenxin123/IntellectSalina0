#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QImage>
#include "wsProtocol.h"

class DataProcessor : public QObject
{
	Q_OBJECT

public:
	explicit DataProcessor(QObject *parent = nullptr);
	~DataProcessor();

	public slots:
	// 处理原始WebSocket数据
	void processRawData(const QByteArray &rawData);

signals:
	// 处理完成的数据信号
	void processedDataReady(const WS::DetectionData& data);
	// 错误信号
	void processingError(const QString &error);

private:
	// 实际的数据处理函数
	WS::DetectionData processDataInternal(const QByteArray &rawData);

private:
	QThread *m_workerThread;  // 内部工作线程
};

#endif // DATAPROCESSOR_H