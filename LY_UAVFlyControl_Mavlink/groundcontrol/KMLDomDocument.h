#ifndef KMLDOMDOCUMENT_H
#define KMLDOMDOCUMENT_H

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QGeoCoordinate>

class MissionItem;
class Vehicle;

/// Used to convert a Plan to a KML document
class KMLDomDocument : public QDomDocument
{

public:
    KMLDomDocument(const QString& name);

    void        appendChildToRoot   (const QDomNode& child);
    QDomElement addPlacemark        (const QString& name, bool visible);
    void        addTextElement      (QDomElement& parentElement, const QString& name, const QString& value);
    QString     kmlColorString      (const QColor& color, double opacity = 1);
    QString     kmlCoordString      (const QGeoCoordinate& coord);
    void        addLookAt           (QDomElement& parentElement, const QGeoCoordinate& coord);

    static const char* balloonStyleName;

protected:
    QDomElement _rootDocumentElement;


private:
    void _addStandardStyles(void);
};

#endif // KMLDOMDOCUMENT_H
