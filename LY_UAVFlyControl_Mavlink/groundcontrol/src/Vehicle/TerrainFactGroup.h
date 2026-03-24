#ifndef TERRAINFACTGROUP_H
#define TERRAINFACTGROUP_H

#include "FactGroup.h"

#include <QGeoCoordinate>

class TerrainFactGroup : public FactGroup
{
    Q_OBJECT

public:
    TerrainFactGroup(QObject* parent = nullptr);

    Q_PROPERTY(Fact* blocksPending  READ blocksPending  CONSTANT)
    Q_PROPERTY(Fact* blocksLoaded   READ blocksLoaded   CONSTANT)

    Fact* blocksPending () { return &_blocksPendingFact; }
    Fact* blocksLoaded  () { return &_blocksLoadedFact; }

    static const char* _blocksPendingFactName;
    static const char* _blocksLoadedFactName;

private:
    Fact _blocksPendingFact;
    Fact _blocksLoadedFact;
};


#endif // TERRAINFACTGROUP_H
