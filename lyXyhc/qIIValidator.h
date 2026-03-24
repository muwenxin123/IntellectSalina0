#ifndef QIIValidator_H
#define QIIValidator_H

#include <QValidator>

class QIIValidator : public QValidator
{
public:
    QIIValidator(int bottom, int top, char separate = '.', unsigned char maxSeg = 2, QObject *parent = nullptr)
        : QValidator(parent), b(bottom), t(top), sep(separate), maxs(maxSeg) { };
    ~QIIValidator() { };

    inline QValidator::State validate(QString &s, int &pos) const override
    {
        QStringList sl = s.split(sep);
        if (sl.size() > maxs)
        {
            return Invalid;
        }

        bool bOk = true;
        for (int i = 0; i < sl.size(); i++)
        {

            if (sl[i].size() < 1)
            {
                continue;
            }

            bool bT;
            int nT = sl[i].toInt(&bT);
            if (bT)
            {
                if (nT < b || nT > t)
                {
                    bOk = false;
                    break;
                }
            }
            else if (b >= 0 || sl[i] != "-")
            {
                bOk = false;
                break;
            }
        }
        if (bOk)
        {
            return Acceptable;
        }
        else
        {
            return Invalid;
        }
    };

private:
    Q_DISABLE_COPY(QIIValidator)

    char sep;
    unsigned char maxs;
    int b;
    int t;
};

#endif
