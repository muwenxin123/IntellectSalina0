/*****************************
 * class：DT_Billboard
 * function：绘制动目标标牌
 * time：2022.3.22
 * create by mah
 * maintenance by mah
 * way：
*****************************/

#include "DT_Billboard.h"

#include <osgWidget/Label>
#include <osg/LineWidth>
#include <osgWidget/WindowManager>
#include <osgViewer/Renderer>
#include <osg/PositionAttitudeTransform>

#include <Windows.h>

#include <QDebug>

#include "CoordinateTransform.h"

DT_Billboard::DT_Billboard(const std::string &id, osg::Node *posnode) :
    osgWidget::Box("billboard", osgWidget::Box::VERTICAL),
    m_positionNode(posnode),
    m_id(id),
    m_simpleLabel(NULL),
    m_isVisible(false),
    m_isItemVisible(false),
    m_isMouseDrag(false),
    m_isMousePush(false)
{
    getBackground()->setColor(1.0, 1.0, 1.0, 0.0);
//    setEventMask(osgWidget::EVENT_ALL);
//    attachMoveCallback();
    
    m_geode = new osg::Geode();
    m_detailLabels.clear();
}

DT_Billboard::~DT_Billboard()
{
    // 拒绝隐式调用
}

void DT_Billboard::setSimpleTitle(const QString &title)
{
    if(!m_simpleLabel)
    {
        m_simpleLabel = new osgWidget::Label(m_id);
        m_simpleLabel->setFont("fonts/simhei.ttf");
        m_simpleLabel->setFontSize(15);
        m_simpleLabel->setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_simpleLabel->setColor(71.0f/255.0f, 166.0f/255.0f, 248.0f/255.0f, 0.6f);
        m_simpleLabel->addSize(100.0f, 20.0f);
        
        m_simpleLabel->setShadow(0.08f);
        //m_simpleLabel->setCanFill(true);
        m_simpleLabel->setImage("data/icon/xsk.png");
        m_simpleLabel->setTexCoord(0.0f, 0.0f, osgWidget::Widget::LOWER_LEFT);
        m_simpleLabel->setTexCoord(1.0f, 0.0f, osgWidget::Widget::LOWER_RIGHT);
        m_simpleLabel->setTexCoord(1.0f, 1.0f, osgWidget::Widget::UPPER_RIGHT);
        m_simpleLabel->setTexCoord(0.0f, 1.0f, osgWidget::Widget::UPPER_LEFT);
    }
    // 解决中文展示问题（采用宽字节）
    std::wstring s_ws;
    gb2312ToUnicode(title.toLocal8Bit().toStdString(), s_ws);
    m_simpleLabel->setLabel(s_ws.c_str());
    
    reDrawBillboard();
}

void DT_Billboard::addItem(const QString &text, int id)
{
    if( m_detailLabels.contains(id) )
        return;
    osg::ref_ptr<osgWidget::Label> label = new osgWidget::Label(m_id + QString::number(id).toStdString());
    label->setFont("fonts/simhei.ttf");
    label->setFontSize(13);
    label->setFontColor(36.0f/255.0f, 36.0f/255.0f, 36.0f/255.0f, 1.0f);
    label->addSize(100.0f, 20.0f);
    label->setColor(1.0f, 1.0f, 1.0f, 0.6f);
    label->setShadow(0.08f);
    //label->setCanFill(true);
    label->setImage("data/icon/xsk.png");
    label->setTexCoord(0.0f, 0.0f, osgWidget::Widget::LOWER_LEFT);
    label->setTexCoord(1.0f, 0.0f, osgWidget::Widget::LOWER_RIGHT);
    label->setTexCoord(1.0f, 1.0f, osgWidget::Widget::UPPER_RIGHT);
    label->setTexCoord(0.0f, 1.0f, osgWidget::Widget::UPPER_LEFT);

    // 解决中文展示问题（采用宽字节）
    std::wstring s_ws;
    gb2312ToUnicode(text.toLocal8Bit().toStdString(), s_ws);
    label->setLabel(s_ws.c_str());
    m_detailLabels.insert(id,label);
    
    reDrawBillboard();
}

int DT_Billboard::getDetailSize()
{
    return m_detailLabels.size();
}

void DT_Billboard::updateItem(int number, const QString &text)
{
    std::wstring s_ws;
    DT_Billboard::gb2312ToUnicode(text.toStdString(), s_ws);
    osg::ref_ptr<osgWidget::Label> label = m_detailLabels.value(number);
    if( label )
        label->setLabel(s_ws.c_str());
}

void DT_Billboard::setPos(osg::Vec3 pos)
{
    m_pos = pos;
    pos = m_pos + m_offset;
    this->setOrigin(pos.x(), pos.y());
    this->update();
}

void DT_Billboard::setOffset(osg::Vec3 offset)
{
    m_offset = offset;
    offset = m_pos + m_offset;
    this->setOrigin(offset.x(), offset.y());
    this->update();
}

void DT_Billboard::setDetailVisible(bool b)
{
    m_isVisible = b;
    if( m_isVisible )
    {
        reDrawBillboard();
    }
    else
    {
        clearDetail();
    }
}

bool DT_Billboard::getDetailVisible()
{
    return m_isVisible;
}

void DT_Billboard::setSimpleBackground(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
    m_simpleLabel->setColor(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
}

osg::Vec3d DT_Billboard::getTargetPos()
{
    osg::PositionAttitudeTransform *pos = dynamic_cast<osg::PositionAttitudeTransform *>(m_positionNode);
    return pos->getPosition();
}

std::string DT_Billboard::getID()
{
    return m_id;
}

void DT_Billboard::createLine(const osg::Vec3 &startPt, const osg::Vec3 &endPt)
{
    if (NULL == m_line)
    {
        //创建标牌和模型之间的连接线
        m_line = new osg::Geometry;
        osg::Vec3Array* vertices = new osg::Vec3Array;
        vertices->push_back(startPt);
        vertices->push_back(endPt);
        m_line->setVertexArray(vertices);

        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
        m_line->setNormalArray(normals);
        m_line->setNormalBinding(osg::Geometry::BIND_OVERALL);

        osg::Vec4Array* colors = new osg::Vec4Array;

        colors->push_back(osg::Vec4(84.0f/255.0f, 213.0f/255.0f, 254.0f/255.0f, 1.0));
        m_line->setColorArray(colors);
        m_line->setColorBinding(osg::Geometry::BIND_OVERALL);

        m_line->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,2));
        m_line->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
        m_line->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(0.8f),osg::StateAttribute::ON);
        m_line->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        m_geode->setName(m_id.c_str());
        m_geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
        m_geode->addDrawable(m_line);
        this->getWindowManager()->addChild(m_geode);
    }
    else
    {   //动态更新线长
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(m_line->getVertexArray());
        (*vertices)[0].set( startPt.x(), startPt.y(), 0.0 );
        (*vertices)[1].set( endPt.x(), endPt.y(), 0.0 );
        vertices->dirty();
        m_line->setVertexArray(vertices);
        m_line->dirtyDisplayList();
    }
}

void DT_Billboard::delLine()
{
    m_geode->removeDrawable(m_line);
}

void DT_Billboard::gb2312ToUnicode(const std::string &src, std::wstring &result)
{
    int n = MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, NULL, 0 );
    result.resize(n);
    ::MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, (LPWSTR)result.c_str(), result.length());
}

void DT_Billboard::unicodeToUTF8(const std::wstring &src, std::string &result)
{
    int n = WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, 0, 0, 0, 0 );
    result.resize(n);
    ::WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, (char*)result.c_str(), result.length(), 0, 0 );
}

void DT_Billboard::gb2312ToUtf8(const std::string &src, std::string &result)
{
    std::wstring strWideChar;
    gb2312ToUnicode(src, strWideChar);
    unicodeToUTF8(strWideChar, result);
}

bool DT_Billboard::mouseDrag(double, double, const osgWidget::WindowManager *)
{
    m_isMouseDrag = true;
    return true;
}

bool DT_Billboard::mousePush(double, double, const osgWidget::WindowManager *)
{
    m_isMousePush = true;
    return true;
}

bool DT_Billboard::mouseRelease(double, double, const osgWidget::WindowManager *)
{
    if(!m_isMouseDrag)
    {
        if(m_isMousePush)
        {
            setDetailVisible(m_isItemVisible);
            m_isItemVisible = !m_isItemVisible;
        }
    }
    m_isMousePush = false;
    m_isMouseDrag = false;

    return true;
}

void DT_Billboard::onMouseEvent(const osgGA::GUIEventAdapter &ea, osgViewer::View *viewer)
{
    int etype = ea.getEventType();
    if(etype == ea.FRAME)
    {
        if (m_positionNode)
        {
            osg::PositionAttitudeTransform *pos = dynamic_cast<osg::PositionAttitudeTransform *>(m_positionNode);
            osg::Vec3 position = pos->getPosition() + osg::Vec3d(0, 0, 0);

			osg::Vec3 renderPos = CoordinateTransform::world2Screen(viewer, position);

            float x = this->getX();
            float y = this->getY();
            float w = this->getWidth();
            float h = this->getHeight();

            float offset = 0.0;
            osg::Vec3 stPt(x + 0.5 * w, y + 0.5 * h, 0);
            if (stPt.y()- renderPos.y()>0.5*h)
            {
                stPt[1] = stPt.y()-0.5*h+offset;
            }
            else if (stPt.y()- renderPos.y()<-0.5*h)
            {
                stPt[1] = stPt.y()+0.5*h+offset;
            }
            if (stPt.x()- renderPos.x()>0.5*w)
            {
                stPt[0] = stPt.x()-0.5*w-offset;
            }
            else if (stPt.x()- renderPos.x()<-0.5*w)
            {
                stPt[0] = stPt.x()+0.5*w-offset;
            }
            // 设置标牌初始位置距离目标的偏移为50
            setPos(renderPos + osg::Vec3d(10, 10, 10));
            createLine(stPt, renderPos);

        }
    }
    if ((etype == ea.PUSH) && ea.getButtonMask() == ea.LEFT_MOUSE_BUTTON)
    {
        if (m_isMousePush)
        {
            m_LBDownPt.set(ea.getX(), ea.getY(), 0);
            m_LastPt = m_LBDownPt;
        }
    }
    if ((etype == ea.DRAG))
    {
        if (m_isMouseDrag)
        {
            osg::Vec3 pt(ea.getX() - m_LastPt[0],  ea.getY() - m_LastPt[1], 0);
            setOffset(m_offset + pt);
            m_LastPt.set(ea.getX(), ea.getY(), 0);
            ea.setHandled(true);
        }
    }
}

void DT_Billboard::clearSimple()
{
    if(m_simpleLabel)
        removeWidget(m_simpleLabel.get());
}

void DT_Billboard::clearDetail()
{
    foreach(int key, m_detailLabels.keys())
    {
        removeWidget(m_detailLabels.value(key).get());
    }
}

void DT_Billboard::reDrawBillboard()
{
    // 谁先add谁在标牌最下面
    clearSimple();
    clearDetail();
    
    QMap<int, osg::ref_ptr<osgWidget::Label> >::const_iterator iter;
    for(iter = --m_detailLabels.constEnd(); iter != --m_detailLabels.constBegin(); iter--)
    {
        addWidget(iter.value().get());
    }
    if( m_simpleLabel)
        addWidget(m_simpleLabel.get());
    
}
