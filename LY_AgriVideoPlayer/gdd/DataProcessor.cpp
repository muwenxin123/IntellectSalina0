#include "DataProcessor.h"
#include <QDebug>
#include <QCoreApplication>
#include "httplib.h"
#include <nlohmann/json.hpp>
#include "../LY_AgriVideoPlayer.h"

using json = nlohmann::json;

DataProcessor::DataProcessor(QObject *parent)
    : QObject(parent)
    , m_workerThread(new QThread(this))
{
    qDebug() << "DataProcessor 构造函数，当前线程:" << QThread::currentThreadId();
    
    // 将 DataProcessor 对象移动到工作线程
    this->moveToThread(m_workerThread);
    
    // 连接工作线程的启动信号
    connect(m_workerThread, &QThread::started, this, []() {
        qDebug() << "DataProcessor 工作线程启动，线程ID:" << QThread::currentThreadId();
    });
    
    // 启动工作线程
    m_workerThread->start();
}

DataProcessor::~DataProcessor()
{
    qDebug() << "DataProcessor 析构函数";
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait(1000);
        if (m_workerThread->isRunning()) {
            m_workerThread->terminate();
            m_workerThread->wait();
        }
    }
}

void DataProcessor::processRawData(const QByteArray &rawData)
{
    qDebug() << "DataProcessor 开始处理数据，线程:" << QThread::currentThreadId() 
             << "数据长度:" << rawData.length();
    
    try {
        // 在工作线程中处理数据
        WS::DetectionData result = processDataInternal(rawData);
        
        // 发射处理完成的数据
        emit processedDataReady(result);
        
        qDebug() << "DataProcessor 数据处理完成，图像尺寸:" << result.image.size()
                 << "多边形数量:" << result.polygons.size();
                 
    } catch (const std::exception& e) {
        QString errorMsg = QString("数据处理错误: %1").arg(e.what());
        qDebug() << errorMsg;
        emit processingError(errorMsg);
    }
}

WS::DetectionData DataProcessor::processDataInternal(const QByteArray &rawData)
{
    WS::DetectionData data;
    
    if (rawData.isEmpty()) {
        qDebug() << "收到空数据";
        return data;
    }
    
    const unsigned char *p = reinterpret_cast<const unsigned char*>(rawData.constData());
    
    // 1. 解析图像数据长度
    if (rawData.size() < 4) {
        throw std::runtime_error("数据长度不足，无法解析图像长度");
    }
    
    int imageLength = *reinterpret_cast<const int*>(p);
    qDebug() << "图像数据长度:" << imageLength;
    
    // 2. 解析图像数据
    if (imageLength > 0) {
        if (rawData.size() < 4 + imageLength) {
            throw std::runtime_error("数据长度不足，无法解析图像数据");
        }
        
        // 从内存加载图像
        QByteArray imageData = QByteArray::fromRawData(
            reinterpret_cast<const char*>(p + 4), imageLength);
        
        bool loadSuccess = data.image.loadFromData(imageData);
        if (!loadSuccess) {
            qDebug() << "图像加载失败，尝试JPEG格式";
            loadSuccess = data.image.loadFromData(imageData, "JPEG");
        }
        
        if (loadSuccess) {
            qDebug() << "图像加载成功，尺寸:" << data.image.size();
        } else {
            qDebug() << "图像加载失败，数据长度:" << imageLength;
        }
        
        p += imageLength;
    }
    p += 4;
    
    // 3. 解析JSON数据
    size_t jsonLength = rawData.size() - (p - reinterpret_cast<const unsigned char*>(rawData.constData()));
    if (jsonLength > 0) {
        try {
            std::string jsonStr(reinterpret_cast<const char*>(p), jsonLength);
            json j = json::parse(jsonStr);
            
            // 调用解析器填充数据
            WS::DetectionParser::parseFromJson(j, data);
            
            qDebug() << "JSON解析成功，元数据尺寸:" << data.metadata.width << "x" << data.metadata.height
                     << "多边形:" << data.polygons.size();
                     
        } catch (const std::exception& e) {
            qDebug() << "JSON解析失败:" << e.what();
        }
    }
    
    return data;
}