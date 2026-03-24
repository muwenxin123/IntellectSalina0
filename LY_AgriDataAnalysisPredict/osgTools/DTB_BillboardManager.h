#ifndef DTB_BILLBOARDMANAGER_H
#define DTB_BILLBOARDMANAGER_H

#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <osg/Group>

#include "DT_Billboard.h"

#include <QMutex>
#include <QVector>

class DTB_BillboardManager : public osgGA::GUIEventHandler
{
protected:
    DTB_BillboardManager();
    ~DTB_BillboardManager();

public:
    static DTB_BillboardManager *instance();

    static void destroy();

public:
    bool handle(const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter&aa);

    // 增
    bool addBoard(const std::string& id, osg::Node *pos, const std::string& simple, std::vector<std::string> detail);

    // 删
    bool delBoard(std::string id);
    
    // 查/改
    DT_Billboard* getBoard(std::string id);

    // 清空
    void clear();

    void setWinManager(void *manager);

    osg::ref_ptr<osgWidget::WindowManager> getWindowManager() const;

private:
    void lock(bool b);

private:
    osgWidget::WindowManager *m_pWindowManager; // 适配属性：窗口管理器

    static DTB_BillboardManager *m_pInstance;

    QVector<void *> m_labelList;

    QMutex m_mutex;         // 互斥量
};

#endif // DTB_BILLBOARDMANAGER_H
