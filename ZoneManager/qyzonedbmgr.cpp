#include "qyzonedbmgr.h"
#include "LyMessageBox.h"
#include "typedefine.h"
#include "QCString.h"
#include "ZoneManager.h"
#include "genstructdef.h"

CQYZoneDBMgr::CQYZoneDBMgr()
{

}

CQYZoneDBMgr::~CQYZoneDBMgr()
{

}

int CQYZoneDBMgr::ReadDatabase(QList<QUYU_Zone> *pZoneArr)
{
    QDb db(ZoneManager::Get().m_strDbIP.data(),
           ZoneManager::Get().m_strDbUser.data(),
           ZoneManager::Get().m_strDbPass.data(),
           ZoneManager::Get().m_strDbName.data(),
           ZoneManager::Get().m_strDbDriver.data());

    if (!db.m_db.isOpen())
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
        return 0;
    }

    QString strSQL = "select QYNM from t_zone where QYDS=0 order by QYNM";
    QDbQuery qry = db.exec(strSQL);
    while (!qry.eof())
    {
        QUYU_Zone zone;
        memset(&zone, 0, sizeof(zone));
        qry >> zone.name;
        pZoneArr->append(zone);
    }

    int type = 0;
    int tianchong = 0;
    int xianxing = 0;

    strSQL = "select QYNM,XIANXING,QYLX,QYXZ,QYSG,QYXG,QYDS,QYCOLOR,TIANCHONG from t_zone where QYDS<>0 order by QYNM";
    QDbQuery qry1 = db.exec(strSQL);
    while (!qry1.eof())
    {
        QUYU_Zone zone;
        memset(&zone, 0, sizeof(zone));
        qry1 >> zone.name >> zone.xianxing >> zone.attr.type >> zone.type >> zone.toph >> zone.both >> zone.data.n >> zone.kycolor >> zone.tianchong;
        ReadZoneDot(tr2(zone.name), &zone, db);
        pZoneArr->append(zone);
    }

    return 1;

}

int CQYZoneDBMgr::ReadZoneDot(const QString &neima, QUYU_Zone *pZone, QDb &db)
{
    if (neima.isEmpty())
    {
        return 0;
    }

    float x, y;
    int xh;
    QString str;
    QString strSQL = QStringLiteral("select QYNM, XH, JD, WD from t_zone_p where QYNM='%1'").arg(neima);

    QDbQuery qry = db.exec(strSQL);
    while (!qry.eof())
    {
        qry >> str >> xh >> x >> y;
        if (xh >= 0 && xh < MAX_ky_point)
        {
            pZone->data.h[xh].x = x;
            pZone->data.h[xh].y = y;
        }
    }
    return 1;

}

int CQYZoneDBMgr::GetMaxID(const QString &tableName )
{
    int id = 0;
    QDb db(ZoneManager::Get().m_strDbIP.data(),
           ZoneManager::Get().m_strDbUser.data(),
           ZoneManager::Get().m_strDbPass.data(),
           ZoneManager::Get().m_strDbName.data(),
           ZoneManager::Get().m_strDbDriver.data());

    if (!db.m_db.isOpen())
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
        return 0;
    }

    QString strSQL = QStringLiteral("select id from %1 order by id DESC").arg(tableName);
    QDbQuery qry = db.exec(strSQL);
    while (!qry.eof())
    {
        qry >> id;
        break;
    }

    return id;

}

bool CQYZoneDBMgr::DeleteZoneDatabase(const QString &ZoneName)
{
    QDb db(ZoneManager::Get().m_strDbIP.data(),
           ZoneManager::Get().m_strDbUser.data(),
           ZoneManager::Get().m_strDbPass.data(),
           ZoneManager::Get().m_strDbName.data(),
           ZoneManager::Get().m_strDbDriver.data());

    if (!db.m_db.isOpen())
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
        return 0;
    }
    QString strSQL;
    strSQL = tr2("delete from t_zone where QYNM ='%1'").arg(ZoneName);
    db.exec(strSQL);

    strSQL = tr2("delete from t_zone_p where QYNM ='%1'").arg(ZoneName);
    db.exec(strSQL);

    return true;

}

bool CQYZoneDBMgr::InsertNewZoneTab(const QUYU_Zone &zone)
{
    QDb db(ZoneManager::Get().m_strDbIP.data(),
           ZoneManager::Get().m_strDbUser.data(),
           ZoneManager::Get().m_strDbPass.data(),
           ZoneManager::Get().m_strDbName.data(),
           ZoneManager::Get().m_strDbDriver.data());

    if (!db.m_db.isOpen())
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
        return 0;
    }

    int i = 0, j = 0;
    char szT[256];
    sprintf(szT, "insert into t_zone(QYNM, XIANXING, QYLX, QYXZ, QYSG, QYXG, QYDS, QYCOLOR, TIANCHONG) values('%s',%d,%d,%d,%f,%f,%d,%d,%d)",
		zone.name, zone.xianxing, zone.attr.type, zone.type, zone.toph, zone.both, zone.data.n, zone.kycolor, zone.tianchong);
    db.exec(tr2(szT));

	QDbQuery qry = db.prepare("insert into t_zone_p(QYNM, XH, JD, WD) values (?,?,?,?)", 4);
	for (j = 0; j < MAX_ky_point && (zone.data.h[j].x != 0 || zone.data.h[j].y != 0); j++)
    {
        qry << (char*)zone.name << j << zone.data.h[j].x << zone.data.h[j].y;
    }

    return true;

}
