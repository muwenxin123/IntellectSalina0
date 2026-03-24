#ifndef USERMANAGESQL_H
#define USERMANAGESQL_H

#include <QDb.h>
#include <QCString.h>

class UserManageServiceSql
{
public:
    UserManageServiceSql();

public:

private:
     QDb* GetDB();

private:
   QDb         *m_db;
   QCString		m_strDbName;
   QCString		m_strDbIP;
   QCString		m_strDbUser;
   QCString		m_strDbPass;
   QCString		m_strDbDriver;
   QCString	    m_strDataDir;
   QCString     m_strConfDir;

};

#endif
