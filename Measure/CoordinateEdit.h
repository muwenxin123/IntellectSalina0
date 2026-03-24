#ifndef COORDINATEEDIT_H
#define COORDINATEEDIT_H

#include <QWidget>
#include <QEvent>

class QLineEdit;
class QLabel;

class CoordinateEdit : public QWidget
{
    Q_OBJECT
public:
    enum Type
    {
        Type_Longitude,
        Type_Lagitude
    };
    CoordinateEdit(QWidget *parent = 0, Type type = Type_Longitude);
    ~CoordinateEdit();

    void SetType(Type type)
    {
        m_type = type;
    }
    QString GetCoordinate()
    {
        return m_coordinate;
    }
    QString GetDegree()
    {
        return m_d;
    }
    QString GetMinute()
    {
        return m_m;
    }
    QString GetSecond()
    {
        return m_s + "." + m_ms;
    }
    double  value();
    void    setValue(double v);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void coordChange(const QString &);

private slots:
    void OnLineEdit(const QString &text);

private:
    Type m_type;
    QLineEdit *m_degree;
    QLineEdit *m_minute;
    QLineEdit *m_second;
    QLineEdit *m_msec;
    QLabel *m_dLab;
    QLabel *m_mLab;
    QLabel *m_sLab;
    QString m_coordinate;
    QString m_d;
    QString m_m;
    QString m_s;
    QString m_ms;
};

#endif
