#include "UserManageServiceSql.h"

UserManageServiceSql::UserManageServiceSql()
{
    QString strIni = m_strConfDir;
    strIni += "/ly.ini";
    QFile file(strIni);
    if (file.exists())
    {
        QSettings ini(strIni, QSettings::IniFormat);
        ini.beginGroup("Database");
        m_strDbName = ini.value("DbName", "LY").toString().toLatin1().data();
        m_strDbIP = ini.value("SvrIP", "127.0.0.1").toString().toLatin1().data();
        m_strDbUser = ini.value("User", "root").toString().toLatin1().data();
        m_strDbPass = ini.value("Password", "root12").toString().toLatin1().data();
        m_strDbDriver = ini.value("Driver", "QMYSQL").toString().toLatin1().data();
        m_db->init(m_strDbIP.data(), m_strDbUser.data(), m_strDbPass.data(), m_strDbName.data(), m_strDbDriver.data());
    }
}

QDb *UserManageServiceSql::GetDB()
{
    return m_db;
}
