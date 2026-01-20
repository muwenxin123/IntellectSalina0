#include "ISelectYoloModelDialog.h"
#include "ui_ISelectYoloModelDialog.h"

#include "TinyXML/tinyxml.h"
#include <QXmlStreamReader>
#include "QCString.h"
#include "QLineSeries"
#include <QAreaSeries>
#include "QValueAxis"
#include "QBarSeries"
#include "QBarSet"
#include "LY_AgriVideoPlayer.h"

ISelectYoloModelDialog::ISelectYoloModelDialog(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::ISelectYoloModelDialog)
    , mianLayout(nullptr)
    , m_selectWdg(nullptr)
    , topWidget(nullptr)
    , buttomWidget(nullptr)
    , ststisticWdg(nullptr)
    , m_charview(nullptr)
    , topflow(nullptr)
    , buttomflow(nullptr)
    , m_topFlowleftMargins(2)
    , m_buttonFlowleftMargins(2)
    //, currSelectButton("110501")
{
    ui->setupUi(this);
    //ui->setupUi(centralWidget());
    mianLayout = new QVBoxLayout(ui->widget);
    this->setLayout(mianLayout);
    //this->setResizable(true);

    m_selectWdg = new QWidget(ui->widget);
    QHBoxLayout* m_btnlayout = new QHBoxLayout(m_selectWdg);
    QPushButton *m_listBtn = new QPushButton(tr2("列表"),m_selectWdg);
    m_btnlayout->addStretch(0);
    m_btnlayout->addWidget(m_listBtn);
    mianLayout->addWidget(m_selectWdg,1);

    QPushButton* m_statisticBtn = new QPushButton(tr2("统计"),m_selectWdg);
    m_btnlayout->addWidget(m_statisticBtn);
    connect(m_listBtn, &QPushButton::clicked, this, &ISelectYoloModelDialog::on_pbtnList_clicked);
    connect(m_statisticBtn, &QPushButton::clicked, this, &ISelectYoloModelDialog::on_pbtnstatistic_clicked);

    topWidget = new QWidget(ui->widget);
    topflow = new FlowLayout(topWidget);
    topflow->setContentsMargins(m_topFlowleftMargins, -1, -1, -1);
    topWidget->setLayout(topflow);

    buttomWidget = new QWidget(ui->widget);
    buttomflow = new FlowLayout(buttomWidget);
    buttomflow->setContentsMargins(m_topFlowleftMargins, -1, -1, -1);
    buttomWidget->setLayout(buttomflow);

    //统计界面
    ststisticWdg = new QWidget(ui->widget);
    mianLayout->addWidget(ststisticWdg,6);
    QHBoxLayout* chatLayout = new QHBoxLayout(ststisticWdg);
    m_chart = new QChart();
    m_chart->setTitle(tr2("识别统计"));
    updateChartView();
    m_charview = new QChartView(m_chart);
    chatLayout->addWidget(m_charview);

    ststisticWdg->hide();

    mianLayout->addWidget(topWidget,3);
    mianLayout->addWidget(buttomWidget,3);
    mianLayout->setContentsMargins(2, 8, -1, -1);

    Init();
}


ISelectYoloModelDialog::~ISelectYoloModelDialog()
{
    delete ui;
}

void ISelectYoloModelDialog::Init()
{
    bool issuccess =loadYoloModelConfig();

    if (issuccess)
    {
        QLayoutItem *child;
        while ((child = topflow->takeAt(0)) != nullptr)
        {
            topflow->removeWidget(child->widget());
            child->widget()->hide();
            child->widget()->setParent(nullptr);
            child->widget()->deleteLater();
            delete child;
        }
    }

    for (auto m_pair : m_yoloModelMap)
    {
        QString buttonTitle = m_pair.first;
        QPushButton *topButton = new QPushButton(buttonTitle,topWidget);
        if (topButton)
        {
            connect(topButton, &QPushButton::clicked, this, [this, buttonTitle] {
                on_topButon_clicked(buttonTitle);
            });
            topButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum));
            topButton->setMinimumSize(QSize(40, 2));
            //topButton->setCheckable(true);
            topflow->addWidget(topButton);
            m_topButtonList.insert(buttonTitle,topButton);
        }
        std::vector<ModelPushButton*>tempVec;
        for (auto yoloinfo : m_pair.second)
        {
            QString mtitle = yoloinfo.ModelTitle;
            ModelPushButton* m_buttomButton = new ModelPushButton(yoloinfo.ModelTitle, yoloinfo.ModelName, yoloinfo.model_cfg, yoloinfo.model_weights, yoloinfo.model_names, buttomWidget);
            connect(m_buttomButton, &ModelPushButton::clicked, this, [this, m_buttomButton]
            {
                on_buttomButton_clicked(m_buttomButton->text());
            });
            m_buttomButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Minimum));
            m_buttomButton->setMinimumSize(QSize(40, 2));
            m_buttomButton->setFixedSize(m_buttomButton->size());
            m_buttomButton->setCheckable(true);
            m_buttomButton->setChecked(false);
            m_buttomButton->hide();
            if (m_buttomButton != nullptr)
            {
                m_buttomButtonList.insert(m_buttomButton, mtitle);
                buttomflow->addWidget(m_buttomButton);
                tempVec.push_back(m_buttomButton);
            }
        }
        m_buttonOfTopMap.insert(std::make_pair(buttonTitle, tempVec));

    }
    QLayoutItem *child;
    while ((child = buttomflow->takeAt(0)) != nullptr)
    {
        buttomflow->removeWidget(child->widget());
    }

    //buttomWidget->setLayout(buttomflow);
}

bool ISelectYoloModelDialog::loadYoloModelConfig()
{
    //读配置文件
    TiXmlDocument pDoc("./Data/YoloModelConfig.xml");
    if (!pDoc.LoadFile())
    {
        return false;
    }
    TiXmlElement *pRoot = pDoc.RootElement();
    if (!pRoot)
    {
        return false;
    }
    TiXmlElement *pCommandNode = pRoot->FirstChildElement("Category");
    while (pCommandNode)
    {
        //YoloModelInfo m_model;
        std::string strCategory1 = pCommandNode->Attribute("type");
        std::string strCount1 = pCommandNode->Attribute("count");
        TiXmlElement *pPositionNode = pCommandNode->FirstChildElement("ModelItem");
        std::vector<YoloModelInfo>tempVec;
        while (pPositionNode)
        {
            std::string strTitle = pPositionNode->Attribute("text");
            std::string strName = pPositionNode->Attribute("Name");
            std::string strcfg = pPositionNode->Attribute("cfg");
            std::string strweights = pPositionNode->Attribute("weights");
            std::string strnames = pPositionNode->Attribute("names");
            YoloModelInfo yoloinfo;
            yoloinfo.ModelTitle = QString::fromStdString(strTitle);
            yoloinfo.ModelName = QString::fromStdString(strName);
            yoloinfo.model_cfg = QString::fromStdString(strcfg);
            yoloinfo.model_weights = QString::fromStdString(strweights);
            yoloinfo.model_names = QString::fromStdString(strnames);
            tempVec.push_back(yoloinfo);
            QString keystr = QString::fromStdString(strCategory1);
            m_yoloModelMap[keystr] = tempVec;
            pPositionNode = pPositionNode->NextSiblingElement("ModelItem");
        }
        pCommandNode = pCommandNode->NextSiblingElement("Category");
    }
    return true;
}

void ISelectYoloModelDialog::updateChartView()
{
    QString name("Series ");
    int nameIndex = 0;
    QLineSeries *lowerSeries = 0;
    //QLineSeries *upperSeries = new QLineSeries(m_chart);

	QReadLocker lk(&m_countLock);
	int index = 0;
	for (auto mpair : m_countModelDetect)
	{
		QBarSeries *mseries = new QBarSeries(m_chart);
		mseries->setName(mpair.first);
		QBarSet *set = new QBarSet(mpair.first);
		*set << mpair.second;
		mseries->append(set);
		m_chart->addSeries(mseries);
		//upperSeries->append(QPoint(index, mpair.second));
	}
    //for (int i = 0; i < 10; i++)
    //{
    //    upperSeries->append(QPoint(i,10-i));
    //}
    //QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
    //m_chart->addSeries(area);
    m_chart->createDefaultAxes();
    //m_chart->axisX()->setRange(0, 10);
    //m_chart->axisY()->setRange(0, 10);
    // Add space to label to add space between labels and axis
    //static_cast<QValueAxis *>(m_chart->axisY())->setLabelFormat("%.1f  ");
}

void ISelectYoloModelDialog::setcurrSelectCamera(QString selectCamera)
{
    currSelectButton = selectCamera;
    if (m_allUAVYoloModelInfoMap.find(selectCamera) != m_allUAVYoloModelInfoMap.end())
    {
        m_YoloModelInfoMap = m_allUAVYoloModelInfoMap[currSelectButton];
    }
    else
    {
        m_YoloModelInfoMap.clear();
    }
	for (auto mitor = m_buttonOfTopMap.begin(); mitor != m_buttonOfTopMap.end();mitor++)
	{
		for (ModelPushButton* mbutton : mitor->second)
		{
			mbutton->setChecked(false);
		}
	}
	updateModelSelectState();
}

void ISelectYoloModelDialog::updateModelSelectState()
{
	if (!m_YoloModelInfoMap.empty())
	{
		for (auto mpair : m_YoloModelInfoMap)
		{
			QString buttonTitle = mpair.second.ModelTitle;
			for (auto mitor = m_buttonOfTopMap.begin(); mitor != m_buttonOfTopMap.end();mitor++)
			{
				bool isfind = false;
				for (ModelPushButton* mbutton : mitor->second)
				{
					if (mbutton->getStrModelTitle() == buttonTitle)
					{
						mbutton->setChecked(true);
						isfind = true;
						break;
					}
				}
				if (isfind)
				{
					break;
				}
			}
		}
	}
	//else
	//{
	//	for (auto mitor = m_buttonOfTopMap.begin(); mitor != m_buttonOfTopMap.end();mitor++)
	//	{
	//		for (ModelPushButton* mbutton : mitor->second)
	//		{
	//			mbutton->setChecked(false);
	//		}
	//	}
	//}
}

void ISelectYoloModelDialog::setModelCount(QString modelTitle)
{
	QReadLocker lk(&m_countLock);
	m_countModelDetect[modelTitle]++;

	updateChartView();
}

std::map<QString, YoloModelInfo> ISelectYoloModelDialog::getSelectModelMap()
{
    QReadLocker lk(&m_readLock);
    return m_allUAVYoloModelInfoMap[currSelectButton];
    //return m_YoloModelInfoMap;
}

void ISelectYoloModelDialog::on_topButon_clicked(QString buttonName)
{

    //for (ModelPushButton* mitem :  m_buttomButtonList.keys())
    //{
    //    mitem->hide();
    //}
    for (auto showItem : m_currShowButtonVec)
    {
        showItem->hide();
        buttomflow->removeWidget(showItem);
    }
    m_currShowButtonVec.clear();
    for (auto itemPair : m_buttonOfTopMap)
    {
        if (itemPair.first == buttonName)
        {
            for (ModelPushButton* btn : itemPair.second)
            {
                //btn->show();
                buttomflow->addWidget(btn);
                btn->show();
                m_currShowButtonVec.push_back(btn);
            }
        }
    }
    buttomflow->activate();
}

void ISelectYoloModelDialog::on_buttomButton_clicked(QString buttonName)
{
    for (ModelPushButton* mitem: m_buttomButtonList.keys())
    {
		//QByteArray tempBName = buttonName.toUtf8();
		//std::string tempstring(tempBName.data(), tempBName.length());
        if (m_buttomButtonList[mitem] == buttonName)
        {
            //if (mitem->isChecked() == false)
            //{
            //    mitem->setChecked(true);
            //}
            //else
            //{
            //    mitem->setChecked(false);
            //}
            //break;
            if (mitem->isChecked())
            {
                YoloModelInfo yoloinfo;
                yoloinfo.ModelTitle = mitem->getStrModelTitle();
                yoloinfo.ModelName = mitem->getStrModelName();
                yoloinfo.model_cfg = mitem->getStrmodel_cfg();
                yoloinfo.model_weights = mitem->getStrmodel_weights();
                yoloinfo.model_names = mitem->getStrmodel_names();
                QReadLocker lk(&m_readLock);
                m_YoloModelInfoMap[buttonName] = yoloinfo;
            }
            else
            {
                auto miter = m_YoloModelInfoMap.find(buttonName);
                if (miter != m_YoloModelInfoMap.end())
                {
                    m_YoloModelInfoMap.erase(miter);
                }
            }
            getModelMapFlag = true;
			bool i = mitem->isChecked();

			// 将摄像头ID转换为视频流ID
			QString videoStreamId;
			if (currSelectButton == "110506") {
				videoStreamId = "v1";
			}
			else if (currSelectButton == "110507") {
				videoStreamId = "v2";
			}
			else if (currSelectButton == "110508") {
				videoStreamId = "v3";
			}
			else if (currSelectButton == "110509") {
				videoStreamId = "v4";
			}
			else if (currSelectButton == "110510") {
				videoStreamId = "v5";
			}
			else {
				videoStreamId = currSelectButton; // 保持原值
			}

			qDebug() << "摄像头ID:" << currSelectButton << "-> 视频流ID:" << videoStreamId;

			//sendModelCommand(videoStreamId, mitem->getStrModelName(), i);
			LY_AgriVideoPlayer::Get().updatejson(videoStreamId, mitem->getStrModelName(), i);
			qDebug() << "modelname:" << mitem->getStrModelName() << "videoid:" << videoStreamId;
			//sendModelCommand(currSelectButton, mitem->getStrModelName(),i );


			// 发送命令
			LY_AgriVideoPlayer::Get().updatejson(videoStreamId, mitem->getStrModelName(), i);

			qDebug() << "? 按钮点击处理完成";
        }
    }
    m_allUAVYoloModelInfoMap[currSelectButton] = m_YoloModelInfoMap;
}

void ISelectYoloModelDialog::on_pbtnList_clicked()
{
    ststisticWdg->hide();
    topWidget->show();
    buttomWidget->show();
}

void ISelectYoloModelDialog::on_pbtnstatistic_clicked()
{
    topWidget->hide();
    buttomWidget->hide();
    ststisticWdg->show();
}


// 初始化网络管理器
void ISelectYoloModelDialog::initNetworkManager()
{
	qDebug() << "开始初始化网络管理器...";

	// 如果已经存在，先清理
	if (m_networkManager) {
		qDebug() << "已存在网络管理器，先清理";
		m_networkManager->deleteLater();
		m_networkManager = nullptr;
	}

	// 创建新的网络管理器
	m_networkManager = new QNetworkAccessManager(this);

	if (m_networkManager) {
		qDebug() << "网络管理器创建成功，地址:" << m_networkManager;
		qDebug() << "父对象:" << m_networkManager->parent();

		// 配置网络管理器
		m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

		// 创建超时定时器
		if (!m_networkTimeoutTimer) {
			m_networkTimeoutTimer = new QTimer(this);
			m_networkTimeoutTimer->setSingleShot(true);
			connect(m_networkTimeoutTimer, &QTimer::timeout, this, [this]() {
				qDebug() << "网络请求超时";
			});
		}

		// 测试网络管理器是否正常工作
		QMetaObject::invokeMethod(this, [this]() {
			qDebug() << "网络管理器测试：可以创建网络请求";
			QNetworkRequest testRequest;
			testRequest.setUrl(QUrl("http://www.example.com"));
			QNetworkReply* testReply = m_networkManager->get(testRequest);
			if (testReply) {
				qDebug() << "测试请求创建成功";
				testReply->deleteLater();
			}
			else {
				qDebug() << "测试请求创建失败";
			}
		}, Qt::QueuedConnection);

	}
	else {
		qCritical() << "网络管理器创建失败!";
		QMessageBox::critical(this, "严重错误", "无法创建网络管理器，网络功能将不可用");
	}
}

// 检查网络管理器是否有效
bool ISelectYoloModelDialog::checkNetworkManager()
{
	if (!m_networkManager) {
		qWarning() << "网络管理器为空，尝试重新初始化";
		initNetworkManager();
	}

	bool isValid = (m_networkManager != nullptr);
	qDebug() << "网络管理器检查:" << (isValid ? "有效" : "无效");

	return isValid;
}


void ISelectYoloModelDialog::sendModelCommand(const QString& videoId,const QString& modelName,bool enable)
{
	// 简单发送JSON到固定IP
	QJsonObject json;
	json["video"] = "";
	json["model"] = modelName;
	json["enable"] = enable;
	json["model(pt)"] = modelName + ".pt";

	QNetworkRequest request;
	request.setUrl(QUrl("http://192.168.1.138:9000/model/control"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	if (m_networkManager == NULL)
	{
		m_networkManager = new QNetworkAccessManager;
	}
	if (m_networkManager != NULL)
	{
		// 检查网络管理器状态
		qDebug() << "网络管理器状态:" << (m_networkManager ? "有效1" : "无效0");
		qDebug() << QString("发送到192.168.1.138: video=%1, model=%2, enable=%3 model(pt)=&4")
			.arg(videoId).arg(modelName).arg(enable).arg(modelName +".pt");
		m_networkManager->post(request, QJsonDocument(json).toJson());
	}
}

