#include "LY_UAVFlyControl_DJI_Dock.h"
#include "LY_UAVFlyControl_DJI_DockInterface.h"

#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"
#include "lyUI/LyWidget.h"
#include "lyUI/LyDialog.h"

#include "IUAVFlyControl_DJI_DockActivator.h"


#include "FlyControl/UAVFlyManage_DJI_DockDialog.h"


#include "djicloude/DJICloudeAPI.h"
#include "djicloude/DJIDockState.h"
#include "djicloude/DJIDockControl.h"

#include "FlyControl/IUAVFlyManage_DJI_SendTask_Dialog.h"
#include "FlyControl/IUAVFlyManage_DJI_WayLine_UploadDialog.h"

#include "FlyControl/IUAVFlyManage_DJI_TakeoffToPoint.h"

#include "IUAVFlyControl_DJI_DockActivator.h"

#include <QMouseEvent>

#include "core/auth/AWSCredentialsProvider.h"

#include "s3/model/ListObjectsV2Request.h"
#include "s3/model/ListObjectsV2Result.h"

#include "s3/model/GetObjectRequest.h"
#include "s3/model/PutObjectRequest.h"

#include <aws/core/auth/AWSCredentialsProvider.h>
#include <fstream>
#include <aws/s3/model/HeadObjectRequest.h>

static bool s_dji_application_menu_control_enabled = true;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

LY_UAVFlyControl_DJI_Dock::LY_UAVFlyControl_DJI_Dock()
    : m_pTab(nullptr)
    , m_pMenu(nullptr)
    , m_pDJICloudeAPI(nullptr)
    , m_pDJIDockState(nullptr)
    , m_pDJIDockControl(nullptr)
    , m_pUAVFlyManage_DJI_DockDialog(nullptr)
    , m_pDJIDockControlLyWidgetDialog(nullptr)
    , m_pUAVFlyManage_MavlinkMainDialog(nullptr)
    , m_pUAVSelectDialog(nullptr)
    , m_pUAVFlyManageDialog(nullptr)
    , m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog(nullptr)
    , m_pUAVFlyManage_MavlinkTaskManageDialog(nullptr)
    , m_pUAVFlyManage_MavlinkMessageIndicatorDialog(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog(nullptr)
, m_pIUAVFlyManage_DJI_SendTask_Dialog(nullptr)
, m_pIUAVFlyManage_DJI_WayLine_UploadDialog(nullptr)
, m_pCDlgTaskLineTransToDJI(nullptr)
, m_pIUAVFlyManage_DJI_TakeoffToPoint(nullptr)
{
	IUAVFlyControl_DJI_DockActivator::Get();


	// read
	{
		QSettings settings("../data/dji/conf/dji_application.ini", QSettings::IniFormat);
		settings.setIniCodec(QTextCodec::codecForName("utf-8"));
		settings.beginGroup("DJI_APPLICATION_SETTINGS");
		if (settings.contains("DJI_APPLICATION_MENU_CONTROL_ENABLED")) {
			bool ok = settings.value("DJI_APPLICATION_MENU_CONTROL_ENABLED").canConvert(QVariant::Type::Bool);
			bool dji_application_menu_control_enabled = settings.value("DJI_APPLICATION_MENU_CONTROL_ENABLED", true).toBool();
			if (ok) {
				s_dji_application_menu_control_enabled = dji_application_menu_control_enabled;
			}
		}
		settings.endGroup();
	}

	// write
	{
		QSettings settings("../data/dji/conf/dji_application.ini", QSettings::IniFormat);
		settings.setIniCodec(QTextCodec::codecForName("utf-8"));
		settings.beginGroup("DJI_APPLICATION_SETTINGS");
		settings.setValue("DJI_APPLICATION_MENU_CONTROL_ENABLED", s_dji_application_menu_control_enabled);
		settings.endGroup();
	}
}

void LY_UAVFlyControl_DJI_Dock::Startup()
{
	IFunctionInterface &ifi = LY_UAVFlyControl_DJI_DockInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr) {
			IUAVFlyControl_DJI_DockActivator::Get().start(pIRegistry);
		}
	}
}

void LY_UAVFlyControl_DJI_Dock::Shutdown()
{
	IFunctionInterface &ifi = LY_UAVFlyControl_DJI_DockInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr) {
			IUAVFlyControl_DJI_DockActivator::Get().stop(pIRegistry);
		}
	}
}

void LY_UAVFlyControl_DJI_Dock::Reset()
{

}

bool LY_UAVFlyControl_DJI_Dock::DrawGraph(QPainter &, IProj *)
{
	return false;
}

bool LY_UAVFlyControl_DJI_Dock::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pIUAVFlyManage_DJI_TakeoffToPoint->isVisible() && m_pIUAVFlyManage_DJI_TakeoffToPoint->b_map_selecting_point)
	{
		m_pIUAVFlyManage_DJI_TakeoffToPoint->DrawTargetPointData(painter, pProj);
	}
	if (m_pIUAVFlyManage_DJI_WayLine_UploadDialog->isVisible())
	{
		m_pIUAVFlyManage_DJI_WayLine_UploadDialog->DrawRouteLine(painter, pProj);
	}

	return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnMouseButtonDown(QMouseEvent *e)
{


	return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnMouseButtonRelease(QMouseEvent *e)
{
	if (m_pIUAVFlyManage_DJI_TakeoffToPoint->b_map_selecting_point)
	{
		m_pIUAVFlyManage_DJI_TakeoffToPoint->b_map_selecting_point = false;

	}
	if (m_pIUAVFlyManage_DJI_TakeoffToPoint->b_simulation_selecting_point)
	{
		m_pIUAVFlyManage_DJI_TakeoffToPoint->b_simulation_selecting_point = false;
	}
	if (m_pIUAVFlyManage_DJI_WayLine_UploadDialog->getWidgetEnable())
	{
		double dlon = 0, dlat = 0;
		m_pProj->Screen2Jwd(e->pos(), dlon, dlat);
		m_pIUAVFlyManage_DJI_WayLine_UploadDialog->AddRoutePoint(dlon, dlat);
	}
	if (m_pIUAVFlyManage_DJI_SendTask_Dialog->b_isMapPoint)
	{
		m_pIUAVFlyManage_DJI_SendTask_Dialog->b_isMapPoint = false;
	}
    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnMouseDoubleClick(QMouseEvent *e)
{

    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnMouseMove(QMouseEvent *e)
{
	double d_flytolon = 0, d_flytolat = 0;
	if (m_pIUAVFlyManage_DJI_TakeoffToPoint->isVisible() && m_pIUAVFlyManage_DJI_TakeoffToPoint->b_map_selecting_point)
	{
		m_pProj->Screen2Jwd(e->pos(), d_flytolon, d_flytolat);
		m_pIUAVFlyManage_DJI_TakeoffToPoint->setTargetPointLocation(d_flytolon, d_flytolat);
	}
	if (m_pIUAVFlyManage_DJI_TakeoffToPoint->isVisible() && m_pIUAVFlyManage_DJI_TakeoffToPoint->b_simulation_selecting_point)
	{
		m_pProj->Screen2Jwd(e->pos(), d_flytolon, d_flytolat);
		m_pIUAVFlyManage_DJI_TakeoffToPoint->setSimulationPointLocation(d_flytolon, d_flytolat);
	}
	if (m_pIUAVFlyManage_DJI_SendTask_Dialog->isVisible() && m_pIUAVFlyManage_DJI_SendTask_Dialog->b_isMapPoint)
	{
		m_pProj->Screen2Jwd(e->pos(), d_flytolon, d_flytolat);
		m_pIUAVFlyManage_DJI_SendTask_Dialog->setMapSelectedPointEdit(d_flytolon, d_flytolat);
	}
    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnCommand( int nID , void *lParam)
{
    switch(nID)
    {
    case 43385:
    {

    }
    break;
    case 44777:
    {

    }
    break;
	case 51009:
	{
		if (m_pDJIDockControl != nullptr && m_pDJIDockControlLyWidgetDialog != nullptr) {
			m_pDJIDockControlLyWidgetDialog->show();
			m_pDJIDockControlLyWidgetDialog->raise();
			m_pDJIDockControl->show();
			m_pDJIDockControl->raise();
		}
	}
	break;
    default:
        break;
    }
    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnCreate( int wParam, QWidget *pTab )
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch(wParam)
    {
    case 0:
    {
    }
    break;
    case 1000:
    {
		m_pTab = pTab;

		// DJIDockControl
		if (m_pDJIDockControlLyWidgetDialog == nullptr && m_pDJIDockControl == nullptr) {

			m_pDJIDockControlLyWidgetDialog = new LyDialog("DJI", pTab);
			m_pDJIDockControl = new DJIDockControl(m_pDJIDockControlLyWidgetDialog);
			if (m_pDJIDockControlLyWidgetDialog != nullptr && m_pDJIDockControl != nullptr) {
				m_pDJIDockControl->init();
				//m_pDJIDockControlLyWidgetDialog->setResizable(true);
				m_pDJIDockControlLyWidgetDialog->resize(m_pDJIDockControl->size());
				QVBoxLayout* verticalLayout = new QVBoxLayout();
				if (verticalLayout) {
					verticalLayout->addWidget(m_pDJIDockControl);
					m_pDJIDockControlLyWidgetDialog->setLayout(verticalLayout);
				}

				m_pUAVFlyManage_DJI_DockDialog = m_pDJIDockControl->pUAVFlyManage_DJI_DockDialog();
			}
		}
		
		if (nullptr == m_pIUAVFlyManage_DJI_SendTask_Dialog)
		{
			m_pIUAVFlyManage_DJI_SendTask_Dialog = new IUAVFlyManage_DJI_SendTask_Dialog(m_pUAVFlyManage_DJI_DockDialog);
			m_pIUAVFlyManage_DJI_SendTask_Dialog->hide();
		}
		if (nullptr == m_pIUAVFlyManage_DJI_WayLine_UploadDialog)
		{
			m_pIUAVFlyManage_DJI_WayLine_UploadDialog = new IUAVFlyManage_DJI_WayLine_UploadDialog(m_pIUAVFlyManage_DJI_SendTask_Dialog);
			m_pIUAVFlyManage_DJI_WayLine_UploadDialog->hide();
		}
		if (nullptr == m_pIUAVFlyManage_DJI_TakeoffToPoint)
		{
			m_pIUAVFlyManage_DJI_TakeoffToPoint = new IUAVFlyManage_DJI_TakeoffToPoint(m_pDJIDockControl);
			m_pIUAVFlyManage_DJI_TakeoffToPoint->hide();
			
		}
    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
    case 4:
    {

    }
        break;
    case 1003:
    {
//         QMenu* pMenu = qobject_cast<QMenu*>(pTab);
//         if (s_dji_application_menu_control_enabled && pMenu != nullptr && m_pMenu == nullptr) {
//             m_pMenu = pMenu;
//             QMenu *pMenuTemp = pMenu->addMenu(tr2("大疆控制"));
//             QAction *action = pMenuTemp->addAction(tr2("大疆机场")); // Dock管理
//             if (action != nullptr) {
//                 QAction::connect(action, &QAction::triggered, [this](bool)
//                 {
//                     LY_UAVFlyControl_DJI_DockInterface::Get().SendMsgToAll(IInterface::MSG_Command, 51009, nullptr);
//                 });
//             }
//         }
    }
        break;
    }
    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_UAVFlyControl_DJI_Dock::OnNetRecv(int , char * )
{

    return false;
}

bool LY_UAVFlyControl_DJI_Dock::NetServerClientSend(const char * const , const unsigned int , const QString & )
{
    return true;
}

void LY_UAVFlyControl_DJI_Dock::SetScreenCenter(const double& dLon, const double& dLat)
{
    if(!m_pProj){
        return ;
    }

    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_UAVFlyControl_DJI_DockInterface::Get().Script(str.toLocal8Bit());
    }

}

QWidget *LY_UAVFlyControl_DJI_Dock::getPTab() const
{
    return m_pTab;
}

const UAVSelectDialog *LY_UAVFlyControl_DJI_Dock::getPUAVSelectDialog() const
{
    return m_pUAVSelectDialog;
}

const UAVFlyManage_MavlinkDialog *LY_UAVFlyControl_DJI_Dock::getPUAVFlyManageDialog() const
{
    return m_pUAVFlyManageDialog;
}

UAVFlyManage_MavlinkMessageIndicatorDialog *LY_UAVFlyControl_DJI_Dock::getPUAVFlyManage_MavlinkMessageIndicatorDialog() const
{
    return m_pUAVFlyManage_MavlinkMessageIndicatorDialog;
}

UAVFlyManage_MavlinkTaskManageDialog * LY_UAVFlyControl_DJI_Dock::getPUAVFlyManage_MavlinkTaskManageDialog() const
{
    return m_pUAVFlyManage_MavlinkTaskManageDialog;
}

IUAVFlyManage_DJI_SendTask_Dialog * LY_UAVFlyControl_DJI_Dock::getPIUAVFlyManage_DJI_SendTask_Dialog() const
{
	return m_pIUAVFlyManage_DJI_SendTask_Dialog;
}

IUAVFlyManage_DJI_WayLine_UploadDialog * LY_UAVFlyControl_DJI_Dock::getPIUAVFlyManage_DJI_WayLine_UploadDialog() const
{
	return m_pIUAVFlyManage_DJI_WayLine_UploadDialog;
}

CDlgTaskLineTransToDJI * LY_UAVFlyControl_DJI_Dock::getPCDlgTaskLineTransToDJI() const
{
	return m_pCDlgTaskLineTransToDJI;
}

IUAVFlyManage_DJI_TakeoffToPoint * LY_UAVFlyControl_DJI_Dock::getPIUAVFlyManage_DJI_TakeoffToPoint() const
{
	return m_pIUAVFlyManage_DJI_TakeoffToPoint;
}

void LY_UAVFlyControl_DJI_Dock::setChangeUAVFlyManage_MavlinkTaskManageDialog(QString str)
{

}

DJICloudeAPI *LY_UAVFlyControl_DJI_Dock::getPDJICloudeAPI() const
{
    if(m_pDJIDockControl != nullptr){
        return m_pDJIDockControl->pDJICloudeAPI();
    }
    return m_pDJICloudeAPI;
}

UAVFlyManage_DJI_DockDialog *LY_UAVFlyControl_DJI_Dock::getPUAVFlyManage_DJI_DockDialog() const
{
    return m_pUAVFlyManage_DJI_DockDialog;
}

void LY_UAVFlyControl_DJI_Dock::setBucketName(const std::string & bucketName)
{
	//s_bucketName = bucketName;
}


bool LY_UAVFlyControl_DJI_Dock::minIO_uploadfile(const std::string& objectKey, const std::string& pathkey, std::string& url, std::string& md5)
{
	QString localFilePath = pathkey.c_str();  // 本地文件

	Aws::SDKOptions m_options;
	Aws::InitAPI(m_options);
	// 初始化 AWS SDK
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.endpointOverride = s_endpointoverride.c_str();  // MinIO 的 S3 API 地址
	clientConfig.region    = s_region.c_str();                        // MinIO 不强制，可随便填
	clientConfig.scheme    = Aws::Http::Scheme::HTTP;            // MinIO 通常用 HTTP
	clientConfig.verifySSL = false;
	//s3Client = new S3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);
	Aws::S3::S3Client s3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);

	bool bIsSuccess = false;
	

	QFile file(localFilePath);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QByteArray fileData = file.readAll();
	file.close();
	
	const std::string& strObjectKey = "wayline/" + objectKey;

	PutObjectRequest putObjectRequest;
	putObjectRequest.WithBucket(s_bucket.c_str()).WithKey(strObjectKey.c_str());
	auto input_data = Aws::MakeShared<Aws::FStream>("PutObject", pathkey.c_str(), std::ios_base::in | std::ios_base::binary);
	putObjectRequest.SetBody(input_data);

	auto outcome = s3Client.PutObject(putObjectRequest);

	if (outcome.IsSuccess()) {
		md5 = outcome.GetResult().GetETag().c_str();

		// 生成预签名 URL
		url = s3Client.GeneratePresignedUrl(s_bucket.c_str(), strObjectKey.c_str(), Aws::Http::HttpMethod::HTTP_GET, 3600).c_str();
		bIsSuccess = true;
	}
	else
	{
		qDebug() <<
			outcome.GetError().GetExceptionName().c_str() << " " <<
			outcome.GetError().GetMessage().c_str();
	}

	Aws::ShutdownAPI(m_options);
	return bIsSuccess;
}

bool LY_UAVFlyControl_DJI_Dock::minIO_downloadfile(const std::string& objectKey, const std::string& localFilePath)
{
	Aws::SDKOptions m_options;
	Aws::InitAPI(m_options);
	// 初始化 AWS SDK
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.endpointOverride = s_endpointoverride.c_str();  // MinIO 的 S3 API 地址
	clientConfig.region = s_region.c_str();                        // MinIO 不强制，可随便填
	clientConfig.scheme = Aws::Http::Scheme::HTTP;            // MinIO 通常用 HTTP
	clientConfig.verifySSL = false;
	//s3Client = new S3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);
	Aws::S3::S3Client s3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);


	Aws::S3::Model::GetObjectRequest object_request;
	object_request.WithBucket(s_bucket.c_str()).WithKey(objectKey.c_str());
	auto get_object_outcome = s3Client.GetObject(object_request);
	if (get_object_outcome.IsSuccess())
	{
		std::ofstream local_file;
		local_file.open(localFilePath.c_str(), std::ios::out | std::ios::binary);
		local_file << get_object_outcome.GetResult().GetBody().rdbuf();
		std::cout << "Done!" << std::endl;
		return true;
	}
	else
	{
		std::cout << "GetObject error: " <<
			get_object_outcome.GetError().GetExceptionName() << " " <<
			get_object_outcome.GetError().GetMessage() << std::endl;
		return false;
	}

	Aws::ShutdownAPI(m_options);
	return true;
}

QStringList LY_UAVFlyControl_DJI_Dock::listObjectsInBucket(const std::string& bucketName)
{
	Aws::SDKOptions m_options;
	Aws::InitAPI(m_options);
	QStringList keyList;
	ListObjectsV2Request request;
	request.SetBucket(bucketName.c_str());
	// 初始化 AWS SDK
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.endpointOverride = s_endpointoverride.c_str();  // MinIO 的 S3 API 地址
	clientConfig.region = s_region.c_str();                        // MinIO 不强制，可随便填
	clientConfig.scheme = Aws::Http::Scheme::HTTP;            // MinIO 通常用 HTTP
	clientConfig.verifySSL = false;
	//s3Client = new S3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);
	Aws::S3::S3Client s3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig
		, false, false);

	auto outcome = s3Client.ListObjectsV2(request);

	if (outcome.IsSuccess()) {
		const auto& result = outcome.GetResult();
		const auto& objects = result.GetContents();

		if (!objects.empty()) {
			for (const auto& obj : objects) {
				keyList << obj.GetKey().c_str();
			}
		}
	}
	Aws::ShutdownAPI(m_options);
	return keyList;
}

bool LY_UAVFlyControl_DJI_Dock::getRouteLineURLorMD5Fingerpoint(const std::string& objectKey, std::string& url, std::string& md5)
{
	Aws::SDKOptions m_options;
	Aws::InitAPI(m_options);
	// 初始化 AWS SDK
	Aws::Client::ClientConfiguration clientConfig;
	clientConfig.endpointOverride = s_endpointoverride.c_str();  // MinIO 的 S3 API 地址
	clientConfig.region = s_region.c_str();                        // MinIO 不强制，可随便填
	clientConfig.scheme = Aws::Http::Scheme::HTTP;            // MinIO 通常用 HTTP
	clientConfig.verifySSL = false;
	//s3Client = new S3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);
	Aws::S3::S3Client s3Client(Aws::Auth::AWSCredentials(s_miniouser.c_str(), s_miniopassword.c_str()), clientConfig, false, false);

	bool bIsSuccess = false;

	Aws::S3::Model::HeadObjectRequest request;
	request.SetBucket(s_bucket.c_str());
	request.SetKey(objectKey.c_str());

	auto get_object_outcome = s3Client.HeadObject(request);

	if (get_object_outcome.IsSuccess())
	{
		QString strMd5 = get_object_outcome.GetResult().GetETag().c_str();
		QString etag = strMd5.mid(1, strMd5.length() - 2);
		md5 = etag.toStdString();
		url = s3Client.GeneratePresignedUrl(s_bucket.c_str(), objectKey.c_str(), Aws::Http::HttpMethod::HTTP_GET, 3600).c_str();
		bIsSuccess = true;
	}
	Aws::ShutdownAPI(m_options);
	return bIsSuccess;
}
