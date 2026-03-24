#ifndef LY_UAVFLYCONTROL_DJI_DOCK_H
#define LY_UAVFLYCONTROL_DJI_DOCK_H

#define USE_IMPORT_EXPORT

#include <QMetaType>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <IProj.h>
#include <QCString.h>
#include "LY_UAVFlyControl_DJI_Dock_global.h"


#include "s3\S3Client.h"
#include "core\Aws.h"

using namespace Aws::S3;
using namespace Aws::S3::Model;

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;
class LyWidget;
class LyDialog;

class UAVFlyManage_MavlinkMainDialog;

class DJICloudeAPI;
class DJIDockState;
class DJIDockControl;
class UAVFlyManage_DJI_DockDialog;

class UAVSelectDialog;
class UAVFlyManage_MavlinkDialog;
class UAVFlyManage_MavlinkTaskManageDialog;
class UAVFlyManage_MavlinkMessageIndicatorDialog;
class UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog;
class IUAVFlyManage_DJI_SendTask_Dialog;
class IUAVFlyManage_DJI_WayLine_UploadDialog;
class CDlgTaskLineTransToDJI;
class IUAVFlyManage_DJI_TakeoffToPoint;

static const std::string s_endpointoverride = "192.168.1.115:9000";
static const std::string s_bucket = "cloud-bucket";
static const std::string s_miniouser = "QFxpF57YW6Dz4dbKw0xx";
static const std::string s_miniopassword = "VrAoDknjN0fEaeLO1tVxAgij3FmY3XDAePuEYKUj";
static const std::string s_region = "cn-beijing";

class LY_UAVFLYCONTROL_DJI_DOCK_EXPORT LY_UAVFlyControl_DJI_Dock
{
public:
    LY_UAVFlyControl_DJI_Dock();
    inline static LY_UAVFlyControl_DJI_Dock &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseButtonRelease(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnMouseMove(QMouseEvent *e);
    bool OnCommand(int nID, void *lParam);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
    bool OnNetRecv(int nLength, char *lpData);
    bool NetServerClientSend(const char * const msg, const unsigned int msglength, const QString &strComponentID);

    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    }
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    }
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    }
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    }
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    }

// 	inline Aws::S3::S3Client getS3Client() 
// 	{
// 		return m_s3Client;
// 	}

    void SetScreenCenter(const double &dLon, const double &dLat);

    QWidget *getPTab() const;

    const UAVSelectDialog *getPUAVSelectDialog() const;

    const UAVFlyManage_MavlinkDialog *getPUAVFlyManageDialog() const;

    UAVFlyManage_MavlinkMessageIndicatorDialog *getPUAVFlyManage_MavlinkMessageIndicatorDialog() const;
    UAVFlyManage_MavlinkTaskManageDialog*      getPUAVFlyManage_MavlinkTaskManageDialog() const;
	IUAVFlyManage_DJI_SendTask_Dialog*			getPIUAVFlyManage_DJI_SendTask_Dialog() const;
	IUAVFlyManage_DJI_WayLine_UploadDialog*		getPIUAVFlyManage_DJI_WayLine_UploadDialog() const;
	CDlgTaskLineTransToDJI*						getPCDlgTaskLineTransToDJI() const;
	IUAVFlyManage_DJI_TakeoffToPoint*			getPIUAVFlyManage_DJI_TakeoffToPoint() const;

   void setChangeUAVFlyManage_MavlinkTaskManageDialog(QString str);


   DJICloudeAPI *getPDJICloudeAPI() const;

   UAVFlyManage_DJI_DockDialog *getPUAVFlyManage_DJI_DockDialog() const;

   //设置bucket名称
   void setBucketName(const std::string& bucketName);
   
   //上传航线
    bool minIO_uploadfile(const std::string& objectKey, const std::string& pathkey, std::string& url, std::string& md5);

   //下载航线
    bool minIO_downloadfile(const std::string& objectKey, const std::string& localFilePath) ;

   //获取桶中所有数据
    QStringList listObjectsInBucket(const std::string& bucketName = "") ;

   //获取桶中指定数据的url和签名
    bool getRouteLineURLorMD5Fingerpoint(const std::string& objectKey, std::string& url, std::string& md5) ;
  
private:
   IProj       *m_pProj;
   QCString     m_strDataDir;
   QCString     m_strConfDir;
private:
    QWidget*                                                    m_pTab;
    QMenu*                                                      m_pMenu;
    DJICloudeAPI*                                               m_pDJICloudeAPI   = nullptr;
    DJIDockState*                                               m_pDJIDockState   = nullptr;
    DJIDockControl*                                             m_pDJIDockControl = nullptr;
    UAVFlyManage_DJI_DockDialog*                                m_pUAVFlyManage_DJI_DockDialog = nullptr;
	LyDialog*                                                   m_pDJIDockControlLyWidgetDialog  = nullptr;

    UAVFlyManage_MavlinkMainDialog*                             m_pUAVFlyManage_MavlinkMainDialog;

    UAVSelectDialog*                                            m_pUAVSelectDialog;
    UAVFlyManage_MavlinkDialog*                                 m_pUAVFlyManageDialog;
    LyDialog*                                                   m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog;
    UAVFlyManage_MavlinkTaskManageDialog*                       m_pUAVFlyManage_MavlinkTaskManageDialog;
    UAVFlyManage_MavlinkMessageIndicatorDialog*                 m_pUAVFlyManage_MavlinkMessageIndicatorDialog;
    LyWidget*                                                   m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog;
    UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog*    m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog;
	IUAVFlyManage_DJI_SendTask_Dialog*							m_pIUAVFlyManage_DJI_SendTask_Dialog;
	IUAVFlyManage_DJI_WayLine_UploadDialog*						m_pIUAVFlyManage_DJI_WayLine_UploadDialog;
	CDlgTaskLineTransToDJI*										m_pCDlgTaskLineTransToDJI;
	IUAVFlyManage_DJI_TakeoffToPoint*							m_pIUAVFlyManage_DJI_TakeoffToPoint;
};

inline LY_UAVFlyControl_DJI_Dock &LY_UAVFlyControl_DJI_Dock::Get()
{
    static LY_UAVFlyControl_DJI_Dock sLY_UAVFlyControl_DJI_Dock;
    return sLY_UAVFlyControl_DJI_Dock;
}

#endif
