#ifndef DT_BILLBOARD_H
#define DT_BILLBOARD_H

#include <QString>
#include <QList>
#include <QMap>

#include <osgWidget/Box>
#include <osgWidget/Label>

#include <string>

class DTB_BillboardLF;
class DTB_BillboardLS;

class DT_Billboard : public osgWidget::Box
{
public:
    explicit DT_Billboard(const std::string &id, osg::Node *posnode);
    ~DT_Billboard();

public:
    // 设置简标牌的标题栏
    void setSimpleTitle(const QString &title);

    // 添加标牌的显示项（详标）
    void addItem(const QString &text, int id);

    int getDetailSize();

    // 更新标牌显示项
    void updateItem(int number, const QString &text);

    // 设置标牌位置（屏幕坐标）
    void setPos(osg::Vec3 pos);

    // 设置标牌偏移量（屏幕坐标）
    void setOffset(osg::Vec3 offset);

    // 设置显隐详细信息
    void setDetailVisible(bool b);

    // 获取详细信息显隐状态
    bool getDetailVisible();
    
    /*******************
     *@biref 设置简标的背景颜色
     *@param unsigned int r : (0~255)
     *@param unsigned int g : (0~255)
     *@param unsigned int b : (0~255)
     *@param unsigned int a : (0~255)
     *@return bool
    **********/
    void setSimpleBackground(unsigned int r, unsigned int g, unsigned int b , unsigned int a);

    osg::Vec3d getTargetPos();

    // 获取标牌ID
    std::string getID();

    // 创建标牌与模型间的连接线
    void createLine(const osg::Vec3 &startPt, const osg::Vec3 &endPt);

    // 删除标牌与模型间的连接线
    void delLine();

    void onMouseEvent(const osgGA::GUIEventAdapter &ea, osgViewer::View *viewer);

private:
    // 移动标牌
    void move();

    void clearSimple();
    
    void clearDetail();
    
    void reDrawBillboard();

public:// 文字编码格式转换
    static void gb2312ToUnicode(const std::string& src, std::wstring& result);

    static void unicodeToUTF8(const std::wstring &src, std::string& result);

    static void gb2312ToUtf8(const std::string& src, std::string& result);


protected:
    bool mouseDrag(double, double, const osgWidget::WindowManager *);

    bool mousePush(double, double, const osgWidget::WindowManager *);

    bool mouseRelease(double, double, const osgWidget::WindowManager *);

private:
    std::string m_id;                                       // 适配属性：唯一标识
    osg::Node *m_positionNode;                              // 适配属性：位置节点

    osg::ref_ptr<osg::Geode> m_geode;                           // 绘制目标与标牌线节点
    osg::ref_ptr<osg::Geometry> m_line;                         // 绘制目标与标牌的线
    osg::ref_ptr<osgWidget::Label> m_simpleLabel;               // 简标
    QMap<int, osg::ref_ptr<osgWidget::Label>> m_detailLabels;   // 详标

    bool m_isVisible;
    bool m_isItemVisible;
    bool m_isMousePush;
    bool m_isMouseDrag;

    osg::Vec3 m_pos;
    osg::Vec3 m_offset;

    osg::Vec3 m_LastPt;
    osg::Vec3 m_LBDownPt;


};

#endif // DT_BILLBOARD_H
