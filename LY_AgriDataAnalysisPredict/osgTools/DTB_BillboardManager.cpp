#include "DTB_BillboardManager.h"

#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>

#include <QThread>

//#include "PreDefine.h"

DTB_BillboardManager* DTB_BillboardManager::m_pInstance = NULL;

DTB_BillboardManager::DTB_BillboardManager()
{
    m_pWindowManager = NULL;
}

DTB_BillboardManager::~DTB_BillboardManager()
{
}

DTB_BillboardManager *DTB_BillboardManager::instance()
{
    if ( NULL == m_pInstance )
    {
        m_pInstance = new DTB_BillboardManager();
    }
    return m_pInstance;
}

void DTB_BillboardManager::destroy()
{
    if ( NULL != m_pInstance )
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

bool DTB_BillboardManager::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    // 解决增加和删除标牌时崩溃问题
    lock(true);
    osgViewer::View *viewer = dynamic_cast<osgViewer::View*>(&aa);
    int nCount = m_labelList.size();
    for (int i = 0; i < nCount; i++)
    {
        DT_Billboard *billboard = (DT_Billboard *)m_labelList[i];
        billboard->onMouseEvent(ea, viewer);
    }
    lock(false);
    return false;
}

bool DTB_BillboardManager::addBoard(const std::string &id, osg::Node *pos, const std::string &simple, std::vector<std::string> detail)
{
    lock(true);
    if( !m_pWindowManager)
    {
        lock(false);
        return false;
    }
    
    for( unsigned int i = 0; i < m_labelList.size(); i++ )
    {
        DT_Billboard *bill = (DT_Billboard *)m_labelList.at(i);
        if(bill->getID() == id)
        {
            lock(false);
            return false;
        }
    }
    DT_Billboard *billboard = new DT_Billboard(id, pos);
    for( int i = 0; i < detail.size(); i++ )
    {
        billboard->addItem(QString::fromStdString(detail.at(i)), billboard->getDetailSize() );
    }
    
    billboard->setSimpleTitle(QString::fromStdString(simple));
    m_pWindowManager->addChild(billboard);
    m_pWindowManager->resizeAllWindows();
    m_labelList.push_back(billboard);
    lock(false);
    return true;
}

bool DTB_BillboardManager::delBoard(std::string id)
{
    lock(true);
    if( !m_pWindowManager)
    {
        lock(false);
        return false;
    }
    QVector<void *>::iterator iter = m_labelList.begin();
    while( iter != m_labelList.end() )
    {
        DT_Billboard* bill = (DT_Billboard *)(*iter);
        if( id == bill->getID() )
        {
            bill->delLine();
            m_pWindowManager->removeChild(bill);
            bill = NULL;
            m_labelList.erase(iter);
            break;
        }
        iter++;
    }

    lock(false);
    return true;
}

DT_Billboard *DTB_BillboardManager::getBoard(std::string id)
{
    QVector<void *>::iterator iter = m_labelList.begin();
    while( iter != m_labelList.end() )
    {
        DT_Billboard* back = (DT_Billboard *)(*iter);
        if( id == back->getID() )
            return back;
        iter++;
    }
    return NULL;
}

void DTB_BillboardManager::clear()
{
    lock(true);
    while( m_labelList.size() > 0 )
    {
        DT_Billboard* back = (DT_Billboard *)m_labelList.back();
        back->delLine();
        m_pWindowManager->removeChild(back);
        back = NULL;
        m_labelList.pop_back();
        
    }
    lock(false);
}

void DTB_BillboardManager::lock(bool b)
{
    b ? m_mutex.lock() : m_mutex.unlock();
}

void DTB_BillboardManager::setWinManager(void *manager)
{
    m_pWindowManager = (osgWidget::WindowManager *)manager;
}

osg::ref_ptr<osgWidget::WindowManager> DTB_BillboardManager::getWindowManager() const
{
    return m_pWindowManager;
}
