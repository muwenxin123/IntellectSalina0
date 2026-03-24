#ifndef FACTSYSTEM_H
#define FACTSYSTEM_H

#include <QGCApplication.h>
#include <QObject>

#include "FactMetaData.h"

class QGCApplication;

class FactSystem : public QObject
{
    Q_OBJECT

public:
    FactSystem(QGCApplication* app);

    // Override from QGCTool
    virtual void setToolbox();

    typedef enum {
        ParameterProvider,
    } Provider_t;

    static const int defaultComponentId = -1;

private:
    static const char* _factSystemQmlUri;   ///< URI for FactSystem QML imports
};

#endif // FACTSYSTEM_H
