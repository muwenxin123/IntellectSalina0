#ifndef LY_MARKMANAGESERVICE_H
#define LY_MARKMANAGESERVICE_H

#include <QCString.h>
#include <QMetaType>
#include "LY_MarkManageService_global.h"
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "IMarkManageImpl.h"
#include "CDlgMarkList.h"
#include "CDlgZoneList.h"
#include "CDlgMarkEdit.h"
#include "CDlgMarkAssign.h"
#include "CDlgMarkImage.h"
#include "IProj.h"
#include "LayerDesc.h"
#include "CreateShape.h"
#include "IInterface.h"
#include "map3d/IEarth.h"
#include "map3d/IGraphics.h"

#include "CDlgMarkPicture.h"
#include "CDlgMarkVideo.h"
#include "CDlgMarkMerge.h"

#include "GetHeight.h"

namespace osg
{
	class Group;
}

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
Q_DECLARE_METATYPE(XygsZC_Mark_01)

class LY_MARKMANAGESERVICESHARED_EXPORT LY_MarkManageService
{
public:
    LY_MarkManageService();
    ~LY_MarkManageService();
    inline static LY_MarkManageService &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseButtonRelease(QMouseEvent *e);
    bool OnMouseMove(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QTabWidget *pTab);
    bool OnTimer(int wParam);
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

    CDlgMarkEdit *GetDlgMark();
    CDlgMarkList *GetDlgMarkList();
    CDlgMarkAssign *GetDlgMarkAssign();
    CDlgMarkImage *GetDlgMarkImage();
    CDlgMarkPicture *GetDlgMarkPicture();
    CDlgMarkVideo *GetDlgMarkVideo();
    CDlgMarkMerge *getDlgMarkMerge();
	CDlgZoneList* GetDlgZoneList();

    void CreateMenu();
    void on_Menu_Picture();
    void on_Menu_Video();
    void on_Menu_Task();
    void on_Menu_Scheme();

    void on_Menu_Destroy();
    void on_Menu_Pressing();
    void on_Menu_Attack();
    void on_Menu_Undestructed();
    void on_Menu_Unsuppressed();
    void Update_MarkState(int Num);

    bool JudgePoint(const QPoint &pt, int &nPoint);
    bool MatchLinePoint(const QPoint &mp, const QPoint &lp);

    bool JudgeMark(const QPoint &pt, std::string &strMarkID);
    bool MatchMarkPoint(const QPoint &mp, const QPoint &lp);
    bool MarkMouseButtonDown(QMouseEvent *e);
    void DrawMarkSelected(QPainter &painter, IProj *pProj);

    void DrawMovingMark(QPainter &painter, IProj *pProj);

    void SetScreenCenter(double &dLon, double &dLat);

    void InitMapLayerDesc();

    void InitModelPatch();

    int AddIcon(double &dLon, double &dLat, ESystemMarkType &eType, ESystemMarkAttribute eAttribute, VecStr &vs);
    bool Add3DIcon(SystemMarkInfo &stInfo);

    int EditIcon(double &dLon, double &dLat, ESystemMarkType &eType, ESystemMarkAttribute eAttribute, VecStr &vs);

    void DeleteIcon(ESystemMarkType &eType, ESystemMarkAttribute eAttribute, int &nId);
    void Delete3DIcon(SystemMarkInfo &stInfo);

    void Clear3DZones();
    void Draw3DZones();

    QString GetMarkTypeString(ESystemMarkType eType);
    QString GetMarkAttributeLayerString(ESystemMarkAttribute eAttribute);
    QString GetMarkAttributeString(ESystemMarkAttribute eAttribute);
    int GetMarkAttributeSymbolIndex(ESystemMarkAttribute eAttribute);
    QString GetMarkSymbolFontName(ESystemMarkType eType);

    void clearDir(const QString &path);

    void createUserMesh2FromShp(IGraphics *pGraphics, std::vector<IUserMesh2 *> &wndVec, ARGB ar, double dLng, double dLat, double dR, double dAltitude = 100000, double dM = 2000);

    void createUserMesh1Polygon(IGraphics *pGraphics, std::vector<IUserMesh1 *> &wndVec, ARGB ar, double *dLng, double *dLat, double *dAlt, int nCount, double dM);

    void createUserMesh1Pie(IGraphics *pGraphics, std::vector<IUserMesh1 *> &wndVec, ARGB ar, double dLng, double dLat, double dAlt, double dRedis, double dStart, double dEnd);

    QVector<QPointF> getCircleQVec(const QPointF &center, int dis, double startAngle, double endAngle, int muti);

	void add3D_fire(std::string MarkID, double lon, double lat, int height, float size = 5.0);
	void del3D_fire(std::string MarkID);

public:
	static CGetHeight s_CGetHeight;
private:
    IProj     *m_pProj;
    QWidget   *m_pWnd;
    QCString  m_strDataDir;
    QCString  m_strConfDir;

    static qnzkna::framework::IRegistry           *s_IRegistryImpl;

    CDlgMarkList     *m_pDlgMarkList;
    CDlgMarkEdit     *m_pDlgMarkEdit;
    CDlgMarkAssign	*m_pDlgMarkAssign;
    CDlgMarkImage	*m_pDlgMarkImage;
    CDlgMarkPicture	*m_pDlgMarkPicture;
    CDlgMarkVideo	*m_pDlgMarkVideo;
    CDlgMarkMerge	*m_pDlgMarkMerge;
	CDlgZoneList	*m_pDlgZoneList;

    CLayerDesc ld;

    struct ModelInfo
    {
        short scale;
        short height;
        short yaw;
        short pitch;
        short roll;
        std::string path;
    };
    std::map<QUuid, IModel *>					m_mapModel;
    std::map<int, ModelInfo>					m_mapModelPath;
    std::map<QUuid, ILabel *>					m_mapLabel;
    std::map<QUuid, std::string>				m_mapLabelPath;

    unsigned int								m_nColor[5];

    std::vector<IUserMesh2 *>					m_3DZoneMesh2Vec;
    std::vector<IUserMesh1 *>					m_3DZoneMesh1Vec;
    std::vector <IUserMesh1 * >					m_3DZoneMesh3Vec;

    int											m_nSelectedID;
    bool										m_bMouseMove = false;

    QMenu										*m_MarkMenu = nullptr;
    QAction										*m_Menu_Picture = nullptr;
    QAction										*m_Menu_Video = nullptr;
    QAction										*m_Menu_Task = nullptr;
    QAction										*m_Menu_Scheme = nullptr;
    QAction                                     *m_Menu_MarkState;

    QMenu                                       *m_Mark_State;
    QAction                                     *m_Menu_Destroy;
    QAction                                     *m_Menu_Pressing;
    QAction                                     *m_Menu_Attack;
    QAction                                     *m_Menu_Undestructed;
    QAction                                     *m_Menu_Unsuppressed;
    std::string									m_SelectedMarkID = "";

	osg::Group*														m_fire_MatrixTransformPtr = nullptr;
};

inline LY_MarkManageService &LY_MarkManageService::Get()
{
    static LY_MarkManageService sLY_MarkManageService;
    return sLY_MarkManageService;
}

#endif
