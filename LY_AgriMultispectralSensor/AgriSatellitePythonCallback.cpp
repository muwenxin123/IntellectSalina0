#include "AgriSatellitePythonCallback.h"

QThreadStorage<CallbackDispatcher*> CallbackWrapper::dispatcherPool;