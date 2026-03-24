#pragma once

#include <QComboBox>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QListView>

class QLineEdit;

struct ItemInfo
{
    int idx;
    QString str;
    QVariant userData;
    bool bChecked;

    ItemInfo()
    {
        idx = -1;
        str = QStringLiteral("");
        userData = QVariant();
        bChecked = false;
    }
};

class KeyPressEater : public QObject
{
    Q_OBJECT
public:
    KeyPressEater(QObject *parent = nullptr) : QObject(parent) {}
    ~KeyPressEater() {}

signals:
    void sigActivated(int idx);

protected:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Space)
            {
                QListView *lstV = qobject_cast<QListView *>(obj);
                if (nullptr != lstV)
                {
                    int idx = lstV->currentIndex().row();
                    if (-1 != idx)
                    {
                        emit sigActivated(idx);
                    }
                }
            }
            else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
            {
                return QObject::eventFilter(obj, event);
            }

            return true;
        }
        else
        {

            return QObject::eventFilter(obj, event);
        }
    }
};

class XComboBox : public QComboBox
{
    Q_OBJECT

public:
    XComboBox(QWidget *parent = Q_NULLPTR);
    ~XComboBox();

    void AddItem(const QString &str, bool bChecked = false, const QVariant &userData = QVariant());
    void AddItems(const QList<ItemInfo> &lstItemInfo);
    void AddItems(const QMap<QString, bool> &mapStrChk);
    void AddItems(const QList<QString> &lstStr);

    void RemoveItem(int idx);

    void Clear();

    QStringList GetSelItemsText();

    QList<ItemInfo> GetSelItemsInfo();

    QString GetItemText(int idx);

    ItemInfo GetItemInfo(int idx);

    int FindItem(const QVariant &Key) const;

signals:

    void showingPopup();

    void hidingPopup();

    void sigItemStateChanged();

protected:
    void showPopup();

    void hidePopup();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    void UpdateText();

private slots:
    void sltActivated(int idx);

private:
    QLineEdit *pLineEdit;
    QListView *pListView;
    QStandardItemModel m_model;
};