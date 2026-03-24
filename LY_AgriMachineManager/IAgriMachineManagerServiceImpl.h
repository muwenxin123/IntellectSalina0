#ifndef IAGRIMACHINEMANAGERSERVICEIMPL_h__
#define IAGRIMACHINEMANAGERSERVICEIMPL_h__

#include <set>
#include <string>

#include <QMutex>
#include <QXmlStreamReader>

#include "DataManage/IAgriMachineManagerService.h"
#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"

namespace qnzkna
{
	namespace AgriMachineManager
	{
		class IAgriMachineManagerServiceImpl : public IAgriMachineManagerService
		{
		public:
			IAgriMachineManagerServiceImpl();
			virtual ~IAgriMachineManagerServiceImpl(void) {}

		private:
			mutable QMutex                       m_mutex;
		};
	}
}
#endif // IAGRIMACHINEMANAGERSERVICEIMPL_h__
