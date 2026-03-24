#pragma once
#ifndef QYZONEDBMGR_H
#define QYZONEDBMGR_H

#include <QList>

#include "genstructdef.h"
#include "QDb.h"

class CQYZoneDBMgr
{
public:
    CQYZoneDBMgr();
    virtual ~CQYZoneDBMgr();

    int ReadDatabase(QList<QUYU_Zone> *pZoneArr);
    int ReadZoneDot(const QString &neima, QUYU_Zone *pZone, QDb &db);
    int GetMaxID(const QString &tableName = QLatin1String("t_battle_zone"));

    bool DeleteZoneDatabase(const QString &ZoneName);
    bool InsertNewZoneTab(const QUYU_Zone &zone);
protected:
private:
};
#endif
