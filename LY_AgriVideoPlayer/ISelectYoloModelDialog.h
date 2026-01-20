#ifndef ISELECTYOLOMODELDIALOG_H
#define ISELECTYOLOMODELDIALOG_H

#include <QWidget>
#include "QPushButton"
#include "ModelPushButton.h"
#include "QBoxLayout"
#include "FlowLayout.h"
#include "QMap"

#include "LyDialog.h"
#include "QMutex"
#include "QReadWriteLock"
#include <QChartGlobal>
#include <QChart>
#include <QChartView>
#include <QGroupBox>
#include <QWebSocket> 
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
QT_CHARTS_USE_NAMESPACE

struct YoloModelInfo
{
    QString ModelTitle;
    QString ModelName;
    QString model_cfg;
    QString model_weights;
    QString model_names;
    YoloModelInfo()
    {
        ModelTitle = "";
        ModelName = "";
        model_cfg = "";
        model_weights = "";
        model_names = "";
    }

};

namespace Ui {
class ISelectYoloModelDialog;
}

class ISelectYoloModelDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ISelectYoloModelDialog(QWidget *parent = nullptr);
    ~ISelectYoloModelDialog();

    void Init();

    bool loadYoloModelConfig();

    void updateChartView();

    void setcurrSelectCamera(QString selectCamera);

	void updateModelSelectState();

    QString getcurrSelectCamera();

	void setModelCount(QString modelTitle);

    std::map<QString, YoloModelInfo> getSelectModelMap();	
private slots:
    void on_topButon_clicked(QString buttonName);

    void on_buttomButton_clicked(QString buttonName);

    void on_pbtnList_clicked();

    void on_pbtnstatistic_clicked();

public:
    QString currSelectButton;

    bool getModelMapFlag = false;
private:
    std::map<QString, std::vector<YoloModelInfo>> m_yoloModelMap;

    QMap<QString, QPushButton* >    m_topButtonList;
    QMap<ModelPushButton *, QString>    m_buttomButtonList;
    std::map<QString, std::vector<ModelPushButton*>> m_buttonOfTopMap;

    std::vector<ModelPushButton*> m_currShowButtonVec;
    //所有选择的模型
    std::map<QString, YoloModelInfo> m_YoloModelInfoMap;

    //多机
    std::map<QString, std::map<QString, YoloModelInfo>> m_allUAVYoloModelInfoMap;
private:
    QVBoxLayout* mianLayout = nullptr;
    const int                                       m_topFlowleftMargins;
    const int                                       m_buttonFlowleftMargins;
    FlowLayout* topflow;
    FlowLayout* buttomflow;
    QGridLayout* mbuttonLayout;
    QWidget* topWidget;
    QWidget *buttomWidget;

    //
    QWidget *m_selectWdg;

    //统计图表
    QWidget *ststisticWdg;
    QChart* m_chart;
    QChartView* m_charview;
	mutable QReadWriteLock m_countLock;
	std::map<QString, int> m_countModelDetect;

    //加锁
    mutable QReadWriteLock m_readLock;
    QMutex m_writeLock;

private:
    Ui::ISelectYoloModelDialog *ui;
private:
	// 确保这里声明了成员变量
	QNetworkAccessManager* m_networkManager = nullptr;  // 明确初始化为nullptr
	QTimer* m_networkTimeoutTimer = nullptr;
	// 添加网络管理器初始化函数
	void initNetworkManager();
	bool checkNetworkManager();

	void sendModelCommand(const QString& videoId,
		const QString& modelName,
		bool enable);

};

#endif // ISELECTYOLOMODELDIALOG_H
