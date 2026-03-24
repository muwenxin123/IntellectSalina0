#include "CDlgMarkImage.h"
#include "ui_CDlgMarkImage.h"
#include "QMenu"
#include "QAction"
#include "QFileDialog"
#include "lyUI/LyMessageBox.h"
#include "QCString.h"
#include <QDateTime>

using namespace qnzkna::MarkManage;
CDlgMarkImage::CDlgMarkImage(QWidget *parent) :
    LyDialog(tr("Mark Picture"), parent),
    ui(new Ui::CDlgMarkImage)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(740, 480);

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->pushButton_3, &QPushButton::pressed, [ = ]()
    {
        this->on_pushButton_3_clicked();
    });
    connect(ui->pushButton_4, &QPushButton::pressed, [ = ]()
    {
        this->on_pushButton_4_clicked();
    });

    connect(ui->listWidget, &QListWidget::currentItemChanged, [ = ](QListWidgetItem * current, QListWidgetItem * previous)
    {
        if (nullptr == current)
        {
            return;
        }
        QString cstrName = current->text();
        this->on_listWidget_itemClicked(current);

    });

    connect(ui->listWidget, &QListWidget::customContextMenuRequested, [ = ](const QPoint & pos)
    {
        QMenu *pMenu = new QMenu;
        QAction *pAddImg = new QAction("Ìí¼Ó", pMenu);
        QAction *pExpImg = new QAction("µ¼³ö", pMenu);
        QAction *pDelImg = new QAction("É¾³ý", pMenu);
        connect(pAddImg, SIGNAL(triggered(bool)), this, SLOT(on_AddImage()));
        connect(pExpImg, SIGNAL(triggered(bool)), this, SLOT(on_ExpImage()));
        connect(pDelImg, SIGNAL(triggered(bool)), this, SLOT(on_DelImage()));
        if (ui->listWidget->itemAt(pos))
        {
            pMenu->addAction(pExpImg);
            pMenu->addAction(pDelImg);
        }
        else
        {
            pMenu->addAction(pAddImg);
            pMenu->addAction(pExpImg);
        }
        pMenu->exec(QCursor::pos());

        delete pMenu;
    });
}

CDlgMarkImage::~CDlgMarkImage()
{
    delete ui;
}

void CDlgMarkImage::InitImgList(const std::string &strMarkID)
{
    initImageControl();

    m_strCurMarkID = strMarkID;
    if ("" == strMarkID)
    {
        return;
    }
    if (nullptr == m_pMarkManage)
    {
        return;
    }
    int nCount = ui->listWidget->count();
    for (int i = nCount - 1; i >= 0; i--)
    {
        QListWidgetItem *pItem = ui->listWidget->takeItem(i);
        if (pItem)
        {
            delete pItem;
            pItem = nullptr;
        }
    }

    std::vector<std::string>	imgNameList;
    bool bGetMarkList = m_pMarkManage->GetMarkImageNamesList(strMarkID, &imgNameList);
    if (!bGetMarkList)
    {
        return;
    }
    m_listOldImages = imgNameList;
    std::vector<std::string>::iterator itrList = imgNameList.begin();
    for (; itrList != imgNameList.end(); itrList ++)
    {
        std::string strName = *itrList;
        ui->listWidget->addItem(strName.c_str());
    }
}

void CDlgMarkImage::setMarkManageImpl(qnzkna::MarkManage::IMarkManageImpl *pMarkImpl)
{
    m_pMarkManage = pMarkImpl;
}

bool CDlgMarkImage::IsImageExistInListWidget(const std::string &strName)
{
    int nCount = ui->listWidget->count();
    for (int i = nCount - 1; i >= 0; i--)
    {
        QListWidgetItem *pItem = ui->listWidget->item(i);
        if (pItem)
        {
            std::string strN = pItem->text().toStdString();
            if (strN == strName)
            {
                return true;
            }
        }
    }
    return false;
}

void CDlgMarkImage::on_listWidget_itemClicked(QListWidgetItem *item)
{

    initImageControl();
    if (nullptr == item)
    {
        return;
    }
    std::string strImgName = item->text().toStdString();
    if (m_pMarkManage)
    {
        QPixmap pixmap;
        bool bGetPixmap = m_pMarkManage->GetMarkImageData(strImgName, &pixmap);
        if (bGetPixmap)
        {
            int nWidth = ui->label->width();
            int nHeight = ui->label->height();
            QPixmap fitPixmap = pixmap.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            ui->label->setPixmap(fitPixmap);

            ui->label_6->setText(QString::number(pixmap.width()));
            ui->label_7->setText(QString::number(pixmap.height()));
            ui->label_10->setText(QString::number(pixmap.depth()));
            ui->label_4->setText(QString::number(pixmap.toImage().sizeInBytes()));
        }
    }
}

void CDlgMarkImage::on_pushButton_3_clicked()
{
    QListWidgetItem *pListItem = ui->listWidget->currentItem();
    if (pListItem)
    {
        std::string strTxt = pListItem->text().toStdString();
        if ("" != strTxt  && m_pMarkManage)
        {
            QPixmap pixmap;
            bool bGetImg = m_pMarkManage->GetMarkImageData(strTxt, &pixmap);
            if (bGetImg)
            {
                int nWidth = ui->label->width();
                int nHeight = ui->label->height();
                QPixmap fitpixmap = pixmap.scaled(nWidth, nHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->label->setPixmap(fitpixmap);
            }
        }
    }
}

void CDlgMarkImage::on_pushButton_4_clicked()
{
    QListWidgetItem *pListItem = ui->listWidget->currentItem();
    if (pListItem)
    {
        std::string strTxt = pListItem->text().toStdString();
        if ("" != strTxt  && m_pMarkManage)
        {
            QPixmap pixmap;
            bool bGetImg = m_pMarkManage->GetMarkImageData(strTxt, &pixmap);
            if (bGetImg)
            {
                int nWidth = ui->label->width();
                int nHeight = ui->label->height();
                QPixmap fitpixmap = pixmap.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                ui->label->setPixmap(fitpixmap);
            }
        }
    }
}

void CDlgMarkImage::on_pushButton_clicked()
{

    QRect rect = ui->label->geometry();
    QPixmap p = this->grab(rect);
    QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    if (!p.save(fileName, "png"))
    {

    }
}

void CDlgMarkImage::on_pushButton_2_clicked()
{

}

void CDlgMarkImage::on_listWidget_itemPressed(QListWidgetItem *item)
{
    if (nullptr == item)
    {
        return;
    }
}

void CDlgMarkImage::on_AddImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "./", tr("Image Files (*.png)"), nullptr, QFileDialog::Option::ReadOnly);
    if (m_pMarkManage)
    {
        QString cstrAbsolutePath = QDir(fileName).absolutePath();
        m_pMarkManage->SetMarkImage(m_strCurMarkID, cstrAbsolutePath);
        std::vector<std::string>	Imglist;
        m_pMarkManage->GetMarkImageNamesList(m_strCurMarkID, &Imglist);
        std::vector<std::string>::iterator itr = Imglist.begin();
        for (; itr != Imglist.end(); itr ++)
        {
            if (!IsImageExistInListWidget(*itr))
            {
                ui->listWidget->addItem((*itr).c_str());
            }
        }
        if (Imglist.size() > m_listOldImages.size())
        {
            ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
            QListWidgetItem *pItem = ui->listWidget->item(ui->listWidget->count() - 1);
            if (pItem)
            {
                std::string strImgName = pItem->text().toStdString();
                QPixmap _Image;
                bool bGetImage = m_pMarkManage->GetMarkImageData(strImgName, &_Image);
                if (bGetImage)
                {
                    int nWidth = ui->label->width();
                    int nHeight = ui->label->height();
                    QPixmap fitPixmap = _Image.scaled(nWidth, nHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    ui->label->setPixmap(fitPixmap);

                    ui->label_6->setText(QString::number(_Image.width()));
                    ui->label_7->setText(QString::number(_Image.height()));
                    ui->label_10->setText(QString::number(_Image.depth()));
                    ui->label_4->setText(QString::number(_Image.toImage().sizeInBytes()));
                }
            }
        }
    }

}

void CDlgMarkImage::on_ExpImage()
{
    int nNum = ui->listWidget->count();
    if (0 == nNum)
    {
        return;
    }
    QListWidgetItem *pListItem = ui->listWidget->currentItem();
    if (pListItem)
    {
        std::string strName = pListItem->text().toLocal8Bit().toStdString();
        if (m_pMarkManage)
        {
            QPixmap pixmap;
            bool bExeResult = m_pMarkManage->GetMarkImageData(strName, &pixmap);
            if (bExeResult)
            {
                QString cstrFileName = "./";
                cstrFileName += strName.c_str();
                cstrFileName.replace(':', '_');
                QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"), cstrFileName, tr("Image Files (*.png)"));
                if (filePath.length())
                {
                    pixmap.save(filePath);
                }
            }
        }
    }
    else
    {

    }
}

void CDlgMarkImage::on_DelImage()
{
    QListWidgetItem *pListItem = ui->listWidget->currentItem();
    if (pListItem)
    {
        std::string strName = pListItem->text().toLocal8Bit().toStdString();
        if (m_pMarkManage)
        {
            bool bExeResult = m_pMarkManage->RemoveMarkImageData(strName);
            if (bExeResult)
            {
                int nRow = ui->listWidget->currentRow();
                pListItem = ui->listWidget->takeItem(nRow);
                delete pListItem;

                initImageControl();
            }
        }
    }
}

void CDlgMarkImage::initImageControl()
{
    ui->label->setPixmap(QPixmap());
    ui->label_4->setText("");
    ui->label_6->setText("");
    ui->label_7->setText("");
    ui->label_10->setText("");
}
