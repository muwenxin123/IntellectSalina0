#if !defined(AFX_QMapOpHandler_H__795BCCDA_BEC7_42D8_9B26_E2B9A9708B5F__INCLUDED_)
#define AFX_QMapOpHandler_H__795BCCDA_BEC7_42D8_9B26_E2B9A9708B5F__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif

#include <qxml.h>
#include <stack>

typedef struct struMenuData MenuData;

class QMapOpHandler : public QXmlDefaultHandler
{
public:

    QMapOpHandler();

    virtual ~QMapOpHandler();

    void loadConfig(const char *lpszConfig);

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);

    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);

    bool characters(const QString &str);

    bool fatalError(const QXmlParseException &exception);

    QString errorString() const;

public:
    inline const QList<MenuData> &getMenuData() const
    {
        return m_lsMenuData;
    };

protected:

private:

public:
    QString		m_strPath;

private:
    QString     m_strCurrentText;
    QString     m_strError;
    QString     m_strCategory;
    QList<MenuData> m_lsMenuData;
};

#endif
