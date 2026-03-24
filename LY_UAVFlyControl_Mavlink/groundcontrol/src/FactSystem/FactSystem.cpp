#include "FactSystem.h"


const char* FactSystem::_factSystemQmlUri = "QGroundControl.FactSystem";

FactSystem::FactSystem(QGCApplication* app)
    : QObject(app)
{
    setToolbox();
}

void FactSystem::setToolbox()
{

}
