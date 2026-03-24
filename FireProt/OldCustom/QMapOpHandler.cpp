#include "QMapOpHandler.h"
#include <QCString.h>
#include <QMenu>
#include <QColor>
#include <QStringList>
#include "MenuData.h"

QMapOpHandler::QMapOpHandler()
{
    m_strPath = ":/images/";
}

QMapOpHandler::~QMapOpHandler()
{
}

void QMapOpHandler::loadConfig(const char *lpszConfig)
{
    QFile file(tr2(lpszConfig));
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QXmlInputSource xmlInputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    reader.parse(xmlInputSource);
}

bool QMapOpHandler::startElement(const QString & ,
                                 const QString & ,
                                 const QString &qName,
                                 const QXmlAttributes &attributes)
{
    if (qName == "Item")
    {
        MenuData md;
        md.nID = attributes.value("ID").toInt();
        md.strText = attributes.value("Text");
        md.strIcon = attributes.value("Icon");
        md.strAction = attributes.value("Action");
        md.stage = attributes.value("Stage").toInt();
        md.showOn3d = attributes.value("ShowOn3D").toInt();
        m_lsMenuData.push_back(md);
    }
    m_strCurrentText.truncate(0);
    return true;
}

bool QMapOpHandler::endElement(const QString & ,
                               const QString & ,
                               const QString &qName)
{
    return true;
}

bool QMapOpHandler::characters(const QString &str)
{
    m_strCurrentText += str;
    return true;
}

bool QMapOpHandler::fatalError(const QXmlParseException &exception)
{
    m_strError = QObject::tr("Parse error at line %1, column %2:\n%3")
                 .arg(exception.lineNumber())
                 .arg(exception.columnNumber())
                 .arg(exception.message());
    return false;
}

QString QMapOpHandler::errorString() const
{
    return m_strError;
}
