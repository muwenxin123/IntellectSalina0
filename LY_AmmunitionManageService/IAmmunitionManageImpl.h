#ifndef LY_AMMUNITIONMANAGEIMPL_H
#define LY_AMMUNITIONMANAGEIMPL_H

#include "DataManage/INebulaAmmunitionManageService.h"
#include "DataManage/NebulaTask/NebulaAmmunition.h"
#include "QMutex"
#include "QSqlDatabase"
#include "QSqlQuery"

namespace qnzkna
{
	namespace AmmunitionManage
	{
		class IAmmunitionManageImpl : public IAmmunitionManageService
		{
		public:
			IAmmunitionManageImpl(void);
			~IAmmunitionManageImpl(void);
		private:
			QSqlDatabase							db;
			QMutex									sMutex;
			CNeubulaAmmunitionVec					m_AmmunitionVec;

			QMutex                                  rMutex;
			CNeubulaAmmunitionInfoVec				m_AmmoVec;

			QMutex									 rAllMutex;
			CNeubulaConntionAmmoVec					 m_AmmoConntion;

			QMutex                                   rAmmoMarkMutex;
			CNeubulaAmmoConntionMarkVec            m_AmmoConntionMarkVec;
		private:
			void InitDataBase();
			bool LoadDataBase();
			bool EditDBData(const CNeubulaAmmunition& data);
			bool AddDBData(const CNeubulaAmmunition& data);
			bool RemoveDBData(int nID);
			bool IsIDExisted(int nID);
			int GetMaxID();

			bool LoadAmmunitionDB();
			bool EditAmmunitionDB(const CNeubulaAmmunitionInfo& data);
			bool AddAmmunitionDB(const CNeubulaAmmunitionInfo& data);
			bool RemoveAmmunitionDB(std::string nID);
			bool IsAmmunitionDBExisted(std::string nID);

			bool LoadAmmoOnPlatDB();

			bool SaveAmmoOnPlatDB(CNeubulaConntionAmmoVec data);
			bool RemoveAmmoOnPlatDB(std::string nTaskID);

			bool LoadAmmoConntionMarkDB();
			bool AddAmmoConntionMarkDB(const CNeubulaAmmoConntionMark& data);
			bool RemoveAmmoConntionMarkDB(std::string sTaskID);
			bool IsAmmoConntionMarkExisted(std::string sTaskID);

		public:

			virtual void LoadDataFromDB();

			virtual CNeubulaAmmunitionVec GetAllAmmunitionVec();

			virtual CNeubulaAmmunition GetAmmunition(const int nID);

			virtual bool AddAmmunition(const CNeubulaAmmunition data);

			virtual bool EditAmmunition(const CNeubulaAmmunition data);

			virtual bool RemoveAmmunition(const int nID);

			virtual bool IsAmmunitionIDExisted(int nID);

			virtual int AutoAmmunitionID();

			virtual void LoadAmmoDB();

			virtual CNeubulaAmmunitionInfoVec  GetAllAmmoVec();

			virtual CNeubulaAmmunitionInfo GetAmmoByID(const std::string nID);

			virtual bool AddAmmo(const CNeubulaAmmunitionInfo data);

			virtual bool EditAmmo(const CNeubulaAmmunitionInfo data);

			virtual bool RemoveAmmo(const std::string nID);

			virtual bool RemoveAllAmmo();

			virtual bool IsAmmoIDExisted(std::string nID);

			virtual CNeubulaConntionAmmoVec GetAmmoOnPlatVec();

			virtual CNeubulaConntionAmmoVec GetAmmunitionConnByTaskID(const std::string nTaskID);

			virtual bool AddAmmunitionConnDB(CNeubulaConntionAmmoVec data);

			virtual void LoadAmmoPlatDB();

			virtual void LoadAmmoMarkDB();

			virtual bool AddAmmoConntionMark(const CNeubulaAmmoConntionMark& data);

			virtual bool RemoveAmmoConntionMark(std::string sTaskID);

			virtual CNeubulaAmmoConntionMarkVec GetAllAmmoConntionMarkVec();

			virtual CNeubulaAmmoConntionMarkVec GetTaskIDByInfo(const std::string sTaskID);

			virtual std::vector<std::string> GetMarkIDByAmmoMode(const CNeubulaAmmoConntionMarkVec data,const std::string sMarkID);

		};
	}
}

#endif