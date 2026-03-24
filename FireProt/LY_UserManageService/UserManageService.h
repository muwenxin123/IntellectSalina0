#ifndef USERMANAGESERVICE_H
#define USERMANAGESERVICE_H

#include "SystemConfig/UserInfo.h"
#include "SystemConfig/IUserManageService.h"
#include <QMutex>

class UserManageService
{
public:
    UserManageService();
    ~UserManageService();

public:

    QMutex                  m_qmutex;
    CUserInfoMap			m_mapUserInfoList;
    CUserTypeMap            m_mapUserTypeList;
    std::string				m_strError;
    std::string				m_strCurUserName;

public:

    void LockMemory();

    void UnLockMemory();

    CUserInfo *GetUserInfo(const std::string &strUserName);

    int SetUserInfo(const CUserInfo &UserInfo);

    int GetUserCount(unsigned int nManagement);

public:
    virtual int Login(CUserInfo &userInfo, const string &strUserName, const string &strPassword);
    virtual int Login(const std::string &strUserName, const std::string &strPassword);
    virtual int GetUserInfo(CUserInfo *pUserInfo);
    virtual int CheckProjectEnable(const std::string &strName);
    virtual int ChangePassword(const std::string &strPassword);
    virtual std::string GetError();

public:
    virtual int ReadXML(const std::string &strFullName = "./data/UserInfoFile.dat");
    virtual int WriteXML(const std::string &strFullName = "./data/UserInfoFile.dat");
    virtual int ReadUserTypeXML(const std::string &strFullName = "./data/UserType.xml");
    virtual int WriteUserTypeXML(const std::string &strFullName = "./data/UserType.xml");
    virtual int GetUserInfoList(CUserInfoMap &mapUserList);
    virtual int GetUserInfo(CUserInfo &rstUserInfo, const std::string &strUserName);
    virtual int IsUserExist(const std::string &strUserName);

    virtual int Register(const CUserInfo &usrTmp);
    virtual int Remove(const std::string &strUserName);
    virtual int ChangeName(const std::string &strNewName, const std::string &strOldName);
    virtual int ChangeManagent(const std::string &strUserName, unsigned int nManagent);
    virtual int ChangePassword(const std::string &strUserName, const std::string &strPassword);
    virtual int ChangeEnableProjectList(const std::string &strUserName, const StringVector &vecProList);
};

#endif
