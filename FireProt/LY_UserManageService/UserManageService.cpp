#include "UserManageService.h"
#include <QDateTime>
#include <QTime>
#include "TinyXML/tinyxml.h"
#include "Common/CommonString.h"
#include <QFile>
#include <QDebug>
#include "QCString.h"

UserManageService::UserManageService()
{
    m_strCurUserName = "";
    m_strError = "";
    m_mapUserInfoList.clear();
    ReadXML("../data/UserInfoFile.dat");
    ReadUserTypeXML("../data/UserType.xml");
}

UserManageService::~UserManageService()
{
    WriteXML("../data/UserInfoFile.dat");
    WriteUserTypeXML("../data/UserType.xml");
}

void UserManageService::LockMemory()
{
    m_qmutex.lock();
}

void UserManageService::UnLockMemory()
{
    m_qmutex.unlock();
}

CUserInfo *UserManageService::GetUserInfo(const string &strUserName)
{
    CUserInfoMapItr itrMap = m_mapUserInfoList.find(strUserName);
    if (itrMap != m_mapUserInfoList.end())
    {
        CUserInfo *pUserInfo = &(itrMap->second);
        return pUserInfo;
    }
    return nullptr;
}

int UserManageService::SetUserInfo(const CUserInfo &UserInfo)
{
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "用户Unknow!";
    if ("" == UserInfo.GetUserName())
    {
        return nRet;
    }
    CUserInfo *pTmpUserInfo = GetUserInfo(UserInfo.GetUserName());
    if (pTmpUserInfo)
    {
        *pTmpUserInfo = UserInfo;
    }
    else
    {
        m_mapUserInfoList.insert(std::make_pair(UserInfo.GetUserName(), UserInfo));
    }
    nRet = RESULT_SUCCESS;
    m_strError = "添加用户信息成功!";
    return nRet;
}

int UserManageService::GetUserCount(unsigned int nManagement)
{
    int nCount = 0;
    CUserInfoMapItr itrMap = m_mapUserInfoList.begin();
    for (; itrMap != m_mapUserInfoList.end(); itrMap ++)
    {
        CUserInfo *pUserInfo = &(itrMap->second);
        if (pUserInfo &&
                (pUserInfo->GetUserManagement() == nManagement))
        {
            nCount ++;
        }
    }
    return nCount;
}

int UserManageService::Login(CUserInfo &userInfo, const string &strUserName, const string &strPassword)
{

    qDebug() << "find user";
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "user not ";
    LockMemory();
    CUserInfo *pUserInfo = GetUserInfo(strUserName);
    qDebug() << tr2(strUserName.c_str()) << tr2(strPassword.c_str());
    if (pUserInfo)
    {
        nRet = RESULT_LOGINERR_PASSWORDERR;
        qDebug() << "密码不正确!";
        m_strError = "pwd not";
        if (pUserInfo->GetUserPassword() == strPassword)
        {
            nRet = RESULT_SUCCESS;
            m_strError = "登录成功!";
            qDebug() << tr2(m_strError.c_str());
            m_strCurUserName = strUserName;
            userInfo = * pUserInfo;
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::Login(const string &strUserName, const string &strPassword)
{

    qDebug() << "find user";
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "user not ";
    LockMemory();
    CUserInfo *pUserInfo = GetUserInfo(strUserName);
    qDebug() << tr2(strUserName.c_str()) << tr2(strPassword.c_str());
    if (pUserInfo)
    {
        nRet = RESULT_LOGINERR_PASSWORDERR;
        qDebug() << "密码不正确!";
        m_strError = "pwd not";
        if (pUserInfo->GetUserPassword() == strPassword)
        {
            nRet = RESULT_SUCCESS;
            m_strError = "登录成功!";
            qDebug() << tr2(m_strError.c_str());
            m_strCurUserName = strUserName;
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::GetUserInfo(CUserInfo *pUserInfo)
{
    int nRet = RESULT_WRITINGERR_NULLPOINT;
    m_strError = "传入数据指针为空!";
    if (!pUserInfo)
    {
        return nRet;
    }
    m_strError = "当前用户Unknow!";
    nRet = RESULT_LOGINERR_USERUNKNOW;
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        m_strError = "当前用户无权操作!";
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        if (EManagement_Normal <= pCurUserInfo->GetUserManagement())
        {
            *pUserInfo = *pCurUserInfo;
            m_strError = "获取当前用户信息成功!";
            nRet = RESULT_SUCCESS;
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::CheckProjectEnable(const string &strName)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "该用户Unknow!";
    if ("" == strName)
    {
        return nRet;
    }
    LockMemory();
    CUserInfo *pUserInfo = GetUserInfo(m_strCurUserName);
    if (pUserInfo)
    {
        m_strError = "该用户无权操作!";
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        if (EManagement_Normal <= pUserInfo->GetUserManagement())
        {
            if (pUserInfo->EnableProject(strName))
            {
                m_strError = "当前用户可以启动该方案!";
                nRet = RESULT_SUCCESS;
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::ChangePassword(const string &strPassword)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strPassword)
    {
        return nRet;
    }

    LockMemory();
    CUserInfo *pCurUerInfo = GetUserInfo(m_strCurUserName);
    if (pCurUerInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "该用户无权操作!";
        if (EManagement_Normal <= pCurUerInfo->GetUserManagement())
        {
            pCurUerInfo->SetUserPassword(strPassword);
            nRet = RESULT_SUCCESS;
            m_strError = "修改密码成功!";
        }
        else if (EManagement_Root == pCurUerInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_ROOTALTERPWD;
            m_strError = "超级用户不能修改自己的密码!";
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

string UserManageService::GetError()
{
    return m_strError;
}

int UserManageService::ReadXML(const string &strFullName)
{

    m_mapUserInfoList.clear();

    QDateTime sysTime = QDateTime::currentDateTime();
    QTime currTime = QTime::currentTime();
    QString qstrPwd = "FKS";
    qstrPwd += QStringLiteral("%1%2%3%4").arg(sysTime.date().year()).arg(sysTime.date().month()).arg(sysTime.date().day()).arg(currTime.hour());
    std::string strPwd = qstrPwd.toStdString();
    CUserInfo rootUserInfo("Root", strPwd, "超级用户", EManagement_Root);
    m_mapUserInfoList.insert(std::make_pair(rootUserInfo.GetUserName(), rootUserInfo));

    qDebug() << "Root" << qstrPwd;

    FILE *pFile = fopen(strFullName.c_str(), "rb");
    string strReadOut;
    if (pFile)
    {
        while (true)
        {
            char cTemp[128] = {0};
            size_t readOutSize = fread(cTemp, 1, 128, pFile);
            if (0 == readOutSize)
            {
                break;
            }
            strReadOut.append(cTemp, readOutSize);
        }
        fclose(pFile);
    }

    for (int iCnt = 0 ; iCnt < strReadOut.size() ; ++iCnt)
    {
        strReadOut[iCnt] = 0xFF - strReadOut[iCnt];
    }

    std::string strTmpFileName = strFullName + ".tmp";
    pFile = fopen(strTmpFileName.c_str(), "wb");
    if (pFile)
    {
        fwrite(strReadOut.c_str(), 1, strReadOut.size(), pFile);
        fclose(pFile);
    }

    TiXmlDocument tiDoc;
    bool bLoad = tiDoc.LoadFile(strTmpFileName.c_str());

    QFile rfile(strTmpFileName.c_str());
    rfile.remove();

    if (!bLoad)
    {
        m_strError = "目录\"" + strFullName + "\"可能不存在!";
        return RESULT_LOADINGERR_DIRECTION;
    }

    TiXmlElement *pRoot = tiDoc.RootElement();
    if (!pRoot)
    {
        m_strError = "Login.xml文件格式不正确!";
        return RESULT_LOADINGERR_FORMAT;
    }

    TiXmlElement *pNodeUserInfoList = pRoot->FirstChildElement("用户信息列表");
    qDebug() << "add to user list" << m_mapUserInfoList.size();
    if (pNodeUserInfoList)
    {
        TiXmlElement *pNodeUserInfo = pNodeUserInfoList->FirstChildElement("用户信息");
        while (pNodeUserInfo)
        {

            std::string strUserName;
            std::string strPassword;
            std::string strManagement;

            if (TilXMLTextToString(strUserName, pNodeUserInfo->Attribute("用户名"))
                    && TilXMLTextToString(strPassword, pNodeUserInfo->Attribute("密码"))
                    && TilXMLTextToString(strManagement, pNodeUserInfo->Attribute("身份")))
            {
                int nManangement = CUserInfo::ManagementToInt(strManagement);

                {
                    std::string strUserID = "";
                    TilXMLTextToString(strUserID, pNodeUserInfo->Attribute("ID"));
                    CUserInfo tmpUserInfo(strUserName, strPassword, strUserID, nManangement);

                    TiXmlElement *pListEnableProject = pNodeUserInfo->FirstChildElement("方案列表");
                    if (pListEnableProject)
                    {

                        TiXmlElement *pNodeEnalbeProject = pListEnableProject->FirstChildElement("方案");
                        while (pNodeEnalbeProject)
                        {

                            std::string strPrjectName;
                            if (TilXMLTextToString(strPrjectName, pNodeEnalbeProject->Attribute("方案名称")))
                            {
                                tmpUserInfo.SetProject(strPrjectName);
                            }

                            pNodeEnalbeProject = pNodeEnalbeProject->NextSiblingElement("方案");
                        }
                    }

                    m_mapUserInfoList.insert(std::make_pair(strUserName, tmpUserInfo));
                    qDebug() << "add to user list" << m_mapUserInfoList.size();
                }
            }

            pNodeUserInfo = pNodeUserInfo->NextSiblingElement("用户信息");
        }
    }
    m_strError = "Login.xml加载成功!";
    return RESULT_SUCCESS;

}

int UserManageService::WriteXML(const string &strFullName)
{
    CUserInfoMap tmpUserInfoList = m_mapUserInfoList;
    CUserInfoMap::iterator itrTmpUser = tmpUserInfoList.find("Root");
    if (itrTmpUser != tmpUserInfoList.end())
    {
        tmpUserInfoList.erase(itrTmpUser);
    }
    TiXmlDocument tiDoc;
    TiXmlDeclaration *ptrDecl = new TiXmlDeclaration("1.0", "GB2312", "");
    if (!ptrDecl)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    tiDoc.LinkEndChild(ptrDecl);
    TiXmlElement *ptrRoot = new TiXmlElement("用户管理");
    if (!ptrRoot)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    tiDoc.LinkEndChild(ptrRoot);
    TiXmlElement *pUserInfoListEle  = new TiXmlElement("用户信息列表");
    if (!pUserInfoListEle)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    ptrRoot->LinkEndChild(pUserInfoListEle);
    CUserInfoMap::iterator itrUser = tmpUserInfoList.begin();
    for (; itrUser != tmpUserInfoList.end() ; itrUser++)
    {
        CUserInfo tmpUserInfo = itrUser->second;

        TiXmlElement *ptrUserInfo = new TiXmlElement("用户信息");
        ptrUserInfo->SetAttribute("用户名",  tmpUserInfo.GetUserName().c_str());
        ptrUserInfo->SetAttribute("密码",	tmpUserInfo.GetUserPassword().c_str());
        ptrUserInfo->SetAttribute("ID",  tmpUserInfo.GetUserID().c_str());
        ptrUserInfo->SetAttribute("身份",	tmpUserInfo.GetUserManagement_Str().c_str());

        TiXmlElement *ptrProjectList = new TiXmlElement("方案列表");
        StringVector::iterator itrProject = tmpUserInfo.GetProjectVec()->begin();
        for (; itrProject != tmpUserInfo.GetProjectVec()->end() ; itrProject++)
        {

            std::string strProjectName = (std::string) * itrProject;
            TiXmlElement *ptrProjectObj = new TiXmlElement("方案");
            ptrProjectObj->SetAttribute("方案名称", strProjectName.c_str());
            ptrProjectList->LinkEndChild(ptrProjectObj);
        }
        ptrUserInfo->LinkEndChild(ptrProjectList);
        pUserInfoListEle->LinkEndChild(ptrUserInfo);
    }

    std::string strTmpFile = strFullName + ".tmp";
    bool bSaveRes = tiDoc.SaveFile(strTmpFile.c_str());
    if (!bSaveRes)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_DIRECTION;
    }

    FILE *pFile = fopen(strTmpFile.c_str(), "rb");
    string strReadOut;
    if (pFile)
    {
        while (true)
        {
            char cTemp[128] = {0};
            size_t readOutSize = fread(cTemp, 1, 128, pFile);
            if (0 == readOutSize)
            {
                break;
            }
            strReadOut.append(cTemp, readOutSize);
        }
        fclose(pFile);
    }

    QFile rfile(strTmpFile.c_str());
    rfile.remove();

    for (int iCnt = 0 ; iCnt < strReadOut.size() ; ++iCnt)
    {
        strReadOut[iCnt] = 0xFF - strReadOut[iCnt];
    }

    pFile = fopen(strFullName.c_str(), "wb");
    if (pFile)
    {
        fwrite(strReadOut.c_str(), 1, strReadOut.size(), pFile);
        fclose(pFile);
    }
    m_strError = "UserInfoFile文件保存成功!";
    return RESULT_SUCCESS;
}

int UserManageService::ReadUserTypeXML(const string &strFullName)
{

    m_mapUserTypeList.clear();

    TiXmlDocument tiDoc;
    bool bLoad = tiDoc.LoadFile(strFullName.c_str());

    if (!bLoad)
    {
        m_strError = "目录\"" + strFullName + "\"可能不存在!";
        return RESULT_LOADINGERR_DIRECTION;
    }

    TiXmlElement *pRoot = tiDoc.RootElement();
    if (!pRoot)
    {
        m_strError = "Login.xml文件格式不正确!";
        return RESULT_LOADINGERR_FORMAT;
    }

    TiXmlElement *pNodeUserInfoList = pRoot->FirstChildElement("用户角色列表");
    qDebug() << "add to user list" << m_mapUserInfoList.size();
    if (pNodeUserInfoList)
    {
        TiXmlElement *pNodeUserInfo = pNodeUserInfoList->FirstChildElement("角色信息");
        while (pNodeUserInfo)
        {

            std::string strUserName;
            std::string strPassword;
            std::string strManagement;

            if (TilXMLTextToString(strUserName, pNodeUserInfo->Attribute("角色名")))
            {

                CUserType tmpUsertype(strUserName);

                TiXmlElement *pListEnableProject = pNodeUserInfo->FirstChildElement("方案列表");
                if (pListEnableProject)
                {

                    TiXmlElement *pNodeEnalbeProject = pListEnableProject->FirstChildElement("方案");
                    while (pNodeEnalbeProject)
                    {

                        std::string strPrjectName;
                        if (TilXMLTextToString(strPrjectName, pNodeEnalbeProject->Attribute("方案名称")))
                        {
                            tmpUsertype.SetProject(strPrjectName);
                        }

                        pNodeEnalbeProject = pNodeEnalbeProject->NextSiblingElement("方案");
                    }
                }

                m_mapUserTypeList.insert(std::make_pair(strUserName, tmpUsertype));
                qDebug() << "add to user list" << m_mapUserInfoList.size();

            }

            pNodeUserInfo = pNodeUserInfo->NextSiblingElement("角色信息");
        }
    }
    m_strError = "Login.xml加载成功!";
    return RESULT_SUCCESS;
}

int UserManageService::WriteUserTypeXML(const string &strFullName)
{
    CUserTypeMap userlist = m_mapUserTypeList;

    TiXmlDocument tiDoc;
    TiXmlDeclaration *ptrDecl = new TiXmlDeclaration("1.0", "GB2312", "");
    if (!ptrDecl)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    tiDoc.LinkEndChild(ptrDecl);
    TiXmlElement *ptrRoot = new TiXmlElement("权限管理");
    if (!ptrRoot)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    tiDoc.LinkEndChild(ptrRoot);
    TiXmlElement *pUserInfoListEle  = new TiXmlElement("用户角色列表");
    if (!pUserInfoListEle)
    {
        m_strError = "文件保存失败!";
        return RESULT_WRITINGERR_NULLPOINT;
    }
    ptrRoot->LinkEndChild(pUserInfoListEle);
    CUserTypeMap::iterator itrUser = userlist.begin();
    for (; itrUser != userlist.end() ; itrUser++)
    {
        CUserType tmpTypeInfo = itrUser->second;

        TiXmlElement *ptrUserInfo = new TiXmlElement("角色信息");
        ptrUserInfo->SetAttribute("角色名",  tmpTypeInfo.GetUserType().c_str());

        TiXmlElement *ptrProjectList = new TiXmlElement("方案列表");
        StringVector::iterator itrProject = tmpTypeInfo.GetProjectVec()->begin();
        for (; itrProject != tmpTypeInfo.GetProjectVec()->end() ; itrProject++)
        {

            std::string strProjectName = (std::string) * itrProject;
            TiXmlElement *ptrProjectObj = new TiXmlElement("方案");
            ptrProjectObj->SetAttribute("方案名称", strProjectName.c_str());
            ptrProjectList->LinkEndChild(ptrProjectObj);
        }
        ptrUserInfo->LinkEndChild(ptrProjectList);
        pUserInfoListEle->LinkEndChild(ptrUserInfo);
    }

    tiDoc.SaveFile(strFullName.c_str());
    m_strError = "UserInfoFile文件保存成功!";
    return RESULT_SUCCESS;
}

int UserManageService::GetUserInfoList(CUserInfoMap &mapUserList)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";

    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户没有该操作权限!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            mapUserList = m_mapUserInfoList;

            CUserInfoMapItr itrMap = mapUserList.find("Root");
            if (itrMap != mapUserList.end())
            {
                mapUserList.erase(itrMap);
            };

            nRet = RESULT_SUCCESS;
            m_strError = "获取用户信息列表成功!";
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::GetUserInfo(CUserInfo &rstUserInfo, const string &strUserName)
{
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strUserName)
    {
        return nRet;
    }
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户没有该权限!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_USERUNKNOW;
            m_strError = "要获取的用户信息不存在!";
            CUserInfo *pGetUserInfo = GetUserInfo(strUserName);
            if (pGetUserInfo)
            {
                rstUserInfo = *pGetUserInfo;
                nRet = RESULT_SUCCESS;
                m_strError = "获取用户信息成功!";
            }
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::IsUserExist(const string &strUserName)
{
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户没有操作权限!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_LOGINERR_USERUNKNOW;
            m_strError = "不存在该用户数据!";
            CUserInfo *pUserInfo = GetUserInfo(strUserName);
            if (pUserInfo)
            {
                nRet = RESULT_SUCCESS;
                m_strError = "该用户存在!";
            }
        }
    }
    UnLockMemory();
    return nRet;
}

int UserManageService::Register(const CUserInfo &usrTmp)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == usrTmp.GetUserName())
    {
        return nRet;
    }

    LockMemory();
    CUserInfo *pUserInfo = GetUserInfo(m_strCurUserName);
    if (pUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户无权操作!";
        if (EManagement_Admin <= pUserInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_USEROCCUPANCY;
            m_strError = "该用户已经存在!";
            if (!GetUserInfo(usrTmp.GetUserName()))
            {
                m_mapUserInfoList.insert(std::make_pair(usrTmp.GetUserName(), usrTmp));
                m_strError = "添加成功";
                nRet = RESULT_SUCCESS;
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::Remove(const string &strUserName)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strUserName)
    {
        return nRet;
    }

    LockMemory();
    CUserInfo *pCurUserInfo  = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "该用户无权操作!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_USERDELETE;
            m_strError = "不能删除当前登录用户!";
            if (strUserName != m_strCurUserName)
            {
                nRet = RESULT_DATAERR_USERUNKNOW;
                m_strError = "要删除用户不存在!";
                CUserInfoMapItr itrMap = m_mapUserInfoList.find(strUserName);
                if (itrMap != m_mapUserInfoList.end())
                {
                    m_mapUserInfoList.erase(itrMap);
                    nRet = RESULT_SUCCESS;
                    m_strError = "删除该用户成功!";
                }
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::ChangeName(const string &strNewName, const string &strOldName)
{
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户无权操作!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_USERUNKNOW;
            m_strError = "要修改的用户不存在!";
            CUserInfoMapItr itrMap = m_mapUserInfoList.find(strOldName);
            if (itrMap != m_mapUserInfoList.end())
            {
                CUserInfo NewUserInfo = itrMap->second;
                nRet = RESULT_DATAERR_USEREXIST;
                m_strError = "新用户名已经被占用!";
                if (!GetUserInfo(strNewName))
                {

                    NewUserInfo.SetUserName(strNewName);
                    m_mapUserInfoList.erase(itrMap);
                    m_mapUserInfoList.insert(std::make_pair(strNewName, NewUserInfo));

                    if (m_strCurUserName == strOldName)
                    {
                        m_strCurUserName = strNewName;
                    }

                    nRet = RESULT_SUCCESS;
                    m_strError = "修改用户名称成功!";
                }
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::ChangeManagent(const string &strUserName, unsigned int nManagent)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strUserName)
    {
        return nRet;
    }

    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "该用户没有该操作权限!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            if (m_strCurUserName == strUserName)
            {
                nRet = RESULT_DATAERR_ALTERMANGEMENT;
                m_strError = "不能修改当前用户(管理员)的权限!";
            }
            else
            {
                CUserInfo *pTmpUserInfo = GetUserInfo(strUserName);
                if (pTmpUserInfo)
                {
                    pTmpUserInfo->SetUserManagement(nManagent);
                    nRet = RESULT_SUCCESS;
                    m_strError = "修改用户权限成功!";
                }
            }
        }
        else if (EManagement_Root == pCurUserInfo->GetUserManagement())
        {
            nRet		= RESULT_DATAERR_ROOTALTERMANAGE;
            m_strError	= "不能修改超级用户的权限!";
            if (pCurUserInfo->GetUserName() != strUserName)
            {
                CUserInfo *pTmpUserInfo = GetUserInfo(strUserName);
                if (pTmpUserInfo)
                {
                    pTmpUserInfo->SetUserManagement(nManagent);
                    nRet		= RESULT_SUCCESS;
                    m_strError	= "修改用户权限成功!";
                }
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::ChangePassword(const string &strUserName, const string &strPassword)
{
    int nRet = RESULT_DATAERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strUserName)
    {
        return nRet;
    }
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户没有操作权限!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_LOGINERR_USERUNKNOW;
            m_strError = "不存在该用户信息!";
            CUserInfo *pUserInfo = GetUserInfo(strUserName);
            if (pUserInfo)
            {
                pUserInfo->SetUserPassword(strPassword);
                nRet = RESULT_SUCCESS;
                m_strError = "修改该用户密码成功!";
            }
        }
        else if (EManagement_Root == pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_DATAERR_ROOTALTERPWD;
            m_strError = "超级用户]不能修改自己的密码!";
            if (strUserName != pCurUserInfo->GetUserName())
            {
                nRet = RESULT_LOGINERR_USERUNKNOW;
                m_strError = "不存在该用户信息!";
                CUserInfo *pUserInfo = GetUserInfo(strUserName);
                if (pUserInfo)
                {
                    pUserInfo->SetUserPassword(strPassword);
                    nRet = RESULT_SUCCESS;
                    m_strError = "修改该用户密码成功!";
                }
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}

int UserManageService::ChangeEnableProjectList(const string &strUserName, const StringVector &vecProList)
{
    int nRet = RESULT_LOGINERR_USERUNKNOW;
    m_strError = "当前用户Unknow!";
    if ("" == strUserName)
    {
        return nRet;
    }
    LockMemory();
    CUserInfo *pCurUserInfo = GetUserInfo(m_strCurUserName);
    if (pCurUserInfo)
    {
        nRet = RESULT_LOGINERR_MENEGENTLESS;
        m_strError = "当前用户无权操作!";
        if (EManagement_Admin <= pCurUserInfo->GetUserManagement())
        {
            nRet = RESULT_LOGINERR_USERUNKNOW;
            m_strError = "不存在该用户数据!";
            CUserInfo *pUserInfo = GetUserInfo(strUserName);
            if (pUserInfo)
            {
                nRet = RESULT_SUCCESS;
                m_strError = "修改用户方案信息成功!";
                if (EManagement_Normal == pUserInfo->GetUserManagement())
                {
                    StringVector *pProVec = pUserInfo->GetProjectVec();
                    *pProVec = vecProList;
                    nRet = RESULT_SUCCESS;
                    m_strError = "修改用户方案信息成功!";
                }
            }
        }
    }
    UnLockMemory();

    if (RESULT_SUCCESS == nRet)
    {
        WriteXML();
    }

    return nRet;
}
