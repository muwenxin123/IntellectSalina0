#ifndef CDLGMARKIMAGE_H
#define CDLGMARKIMAGE_H

#include <QWidget>
#include "lyUI/LyDialog.h"
#include "QListWidgetItem"
#include "IMarkManageImpl.h"

namespace Ui
{
class CDlgMarkImage;
}
class IMarkManageImpl;
class CDlgMarkImage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMarkImage(QWidget *parent = nullptr);
    ~CDlgMarkImage();

    void InitImgList(const std::string &strMarkID);
    void setMarkManageImpl(qnzkna::MarkManage::IMarkManageImpl *pMarkImpl);
    bool IsImageExistInListWidget(const std::string &strName);
private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);
    void on_AddImage();
    void on_ExpImage();
    void on_DelImage();
private:
    Ui::CDlgMarkImage *ui;
    qnzkna::MarkManage::IMarkManageImpl	*m_pMarkManage;
    std::string		m_strCurMarkID;
    std::vector<std::string>	m_listOldImages;
    void initImageControl();
};

#endif
