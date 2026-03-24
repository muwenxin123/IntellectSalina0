#ifndef KMLPLANDOMDOCUMENT_H
#define KMLPLANDOMDOCUMENT_H

#include <QObject>
#include "KMLDomDocument.h"

class MissionItem;
class Vehicle;
class VisualMissionItem;

/// Used to convert a Plan to a KML document
class KMLPlanDomDocument : public KMLDomDocument
{

public:
    KMLPlanDomDocument();

    void addMission(Vehicle* vehicle, QList<VisualMissionItem*>* visualItems, QList<MissionItem*> rgMissionItems);

    static const char* surveyPolygonStyleName;

private:
    void _addStyles         (void);
    void _addFlightPath     (Vehicle* vehicle, QList<MissionItem*> rgMissionItems);
    void _addComplexItems   (QList<VisualMissionItem*>* visualItems);

    static const char* _missionLineStyleName;
};


#endif // KMLPLANDOMDOCUMENT_H
