#ifndef XYGSZC_02_PROCESS_H
#define XYGSZC_02_PROCESS_H

#include <QObject>

#include <map>
#include <QMutex>

#include "DataManage/Common/TypeDefine_Node.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"

#undef USING_LY_CCIP_80MM_DEFINE
#ifdef USING_LY_CCIP_80MM_DEFINE
    #include "ly_ccip.h"
#endif

class XygsZC_02_Process : public QObject
{
    Q_OBJECT
public:
    explicit XygsZC_02_Process(QObject *parent = nullptr);

    virtual ~XygsZC_02_Process() {}

signals:
    void Process_XygsZC_02_Sig(const unsigned long long unNodeID, const XygsZC_02 xygsZC_02);

public slots:
    void Process_XygsZC_02_Slot(const unsigned long long unNodeID, const XygsZC_02 xygsZC_02);

public:
    QMutex                                  m_MutexXygsZC_02Map;
    std::map<unsigned long long, XygsZC_02> m_XygsZC_02Map;
};

#endif
