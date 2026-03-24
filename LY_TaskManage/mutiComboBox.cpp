#include "mutiComboBox.h"

#include <QCheckBox>
#include <QDebug>

MutiComboBox::MutiComboBox(QWidget *parent)
    : QComboBox(parent)
{
    pListWidget = new QListWidget(this);
    pListWidget->setObjectName("MutiComboBoxListWidget");
    pLineEdit = new QLineEdit(this);

    this->setLineEdit(pLineEdit);
    pLineEdit->setReadOnly(true);
    connect(pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

MutiComboBox::MutiComboBox(const QMap<int, QString> &mapData, QWidget *parent)
    : QComboBox(parent)
{
    pListWidget = new QListWidget(this);
    pLineEdit = new QLineEdit(this);
    QMapIterator<int, QString> it(mapData);
    while (it.hasNext())
    {
        it.next();
        QListWidgetItem *pItem = new QListWidgetItem(pListWidget);
        pListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.value());
        pListWidget->addItem(pItem);
        pListWidget->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    }

    this->setModel(pListWidget->model());
    this->setView(pListWidget);
    this->setLineEdit(pLineEdit);
    pLineEdit->setReadOnly(true);
    connect(pLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

MutiComboBox::~MutiComboBox()
{
    delete pListWidget;
    delete pLineEdit;
}
void MutiComboBox::setModelData(const QMap<int, QString> &mapData)
{
    int count = pListWidget->count();
    for (int row = 0; row < count; ++row)
    {
        QListWidgetItem *pItem = pListWidget->takeItem(0);
        delete pItem;
    }

    QMapIterator<int, QString> it(mapData);
    while (it.hasNext())
    {
        it.next();
        QListWidgetItem *pItem = new QListWidgetItem(pListWidget);
        pListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.value());
        pListWidget->addItem(pItem);
        pListWidget->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    }
    this->setModel(pListWidget->model());
    this->setView(pListWidget);
}

void MutiComboBox::setCheckedItems(const QString &data)
{
    clear();

    if (data.isEmpty())
    {
        return;
    }

    int count = pListWidget->count();
    QStringList list(data.split(";"));
    QStringListIterator it(list);
    while (it.hasNext())
    {
        QString da = it.next();
        for (int i = 0; i < count; ++i)
        {
            QListWidgetItem *pItem = pListWidget->item(i);
            if (pItem->data(Qt::UserRole).toString() == da)
            {
                QWidget *pWidget = pListWidget->itemWidget(pItem);
                QCheckBox *pCheckBox = (QCheckBox *) pWidget;
                pCheckBox->setChecked(true);
                break;
            }
        }
    }
}
QString MutiComboBox::getSelectedItemDatas()
{
    int nCount = pListWidget->count();
    QString strSeltectedData("");
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *) pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pItem->data(Qt::UserRole).toString().trimmed();
            strSeltectedData.append(strText).append(";");
        }
    }

    if (strSeltectedData.endsWith(";"))
    {
        strSeltectedData.remove(strSeltectedData.count() - 1, 1);
    }
    return strSeltectedData;
}

void MutiComboBox::hidePopup()
{
    QAbstractItemView *pItemView = this->view();
    int ItemViewY = pItemView->height();
    int height = this->height();

    int curY = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();

    int curX = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
    int ItemViewX = pItemView->width();
    int Width = this->width();

    if (curY < height || curY > ItemViewY + height || curX < 0 || curX > ItemViewX)
    {
        QComboBox::hidePopup();

        emit(hidePopuped());
    }
}

void MutiComboBox::stateChanged(int state)
{
    bSelected = true;
    QString strSelectedData("");
    strSelectedText.clear();
    QObject *object = QObject::sender();
    QCheckBox *pSenderCheckBox = static_cast<QCheckBox *>(object);
    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *) pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(";");
        }
    }

    if (strSelectedData.endsWith(";"))
    {
        strSelectedData.remove(strSelectedData.count() - 1, 1);
    }

    if (!strSelectedData.isEmpty())
    {
        strSelectedText = strSelectedData;
        pLineEdit->setText(strSelectedData);
        pLineEdit->setToolTip(strSelectedData);
    }
    else
    {
        pLineEdit->clear();
    }
    bSelected = false;
}

void MutiComboBox::textChanged(const QString &text)
{
    if (!bSelected)
    {
        pLineEdit->setText(strSelectedText);
    }
}

void MutiComboBox::clear()
{
    int nCount = pListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = pListWidget->item(i);
        QWidget *pWidget = pListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *) pWidget;
        if (pCheckBox->isChecked())
        {
            pCheckBox->setChecked(false);
        }
    }
    strSelectedText = "";
    pLineEdit->setText("");
}

