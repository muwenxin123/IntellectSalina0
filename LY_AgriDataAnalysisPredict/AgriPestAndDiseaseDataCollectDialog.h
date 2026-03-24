#ifndef AGRIPESTANDDISEASEDATACOLLECTDIALOG_H_
#define AGRIPESTANDDISEASEDATACOLLECTDIALOG_H_
 
#include <QWidget>
#include <QTableWidget>

#include "LyDialog.h"
#include "AgriDataAnalysisPredictDecl.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "IAgriDataAnalysisPredictActivator.h"
#include "ImgRecognize/AgriYoloRecognize.h"


namespace Ui {
	class AgriPestAndDiseaseDataCollectDialog;
}

class AgriPestAndDiseaseDataCollectDialog : public LyDialog
{
	Q_OBJECT

public:
	/* 添加数据集 */
	explicit AgriPestAndDiseaseDataCollectDialog(DataCollectType collectType, AgriPestDataset dataset, QWidget *parent = nullptr);
	/* 更新数据集 */
	explicit AgriPestAndDiseaseDataCollectDialog(DataCollectType collectType, AgriPestDataset dataset, AgriPestCollection collect, QWidget *parent = nullptr);
	/* 查看数据集 */
	explicit AgriPestAndDiseaseDataCollectDialog(AgriPestDataset dataset, AgriPestCollection collect, DataCollectType collectType, QWidget *parent = nullptr);
	~AgriPestAndDiseaseDataCollectDialog();

public:
	/*!!! Dialog仅支持第一次初始化，不支持中间切换 !!!*/
	void InitActiveFieldZone(const qnzkna::ZoneManage::CZoneBase& zone);
	inline bool isInitFieldZone() { return m_bInitZone; };
	inline bool isPositionPick() { return m_bPick; };
	void updateTargetPoint(double dLon, double dLat, float fAlt);

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void Init();
	void InitDisplayCombox();
	void InitDisplayInfo();
	void AddDataCollection(AgriPestCollectionPtr pestCollection);
	void UpdateDataCollection(AgriPestCollectionPtr pestCollection);

signals:
	void addDataCollectionFinish(AgriPestCollectionPtr pestCollection);
	void updateDataCollectionFinish(AgriPestCollectionPtr pestCollection);

public slots:
	void on_btnOK_clicked();
	void on_btnCancel_clicked();
	void on_btnPick_clicked();
	void on_btnSelect_clicked();

	void on_combo_pest_category_currentTextChanged(int index);
	void on_combo_pest_stage_currentTextChanged(int index);
	void on_combo_pest_level_currentTextChanged(int index);
	void on_comboBox_sensor_currentTextChanged(int index);

private:
	enum CreateDialogType
	{
		CREATE_TYPE_UNKNOW,		// 未知
		CREATE_TYPE_ADD,		// 添加
		CREATE_TYPE_UPDATE,		// 更新
		CREATE_TYPE_VIWE,		// 查看

	};

	CreateDialogType						m_eCerateType{ CREATE_TYPE_UNKNOW };
	DataCollectType							m_eCollectType = DataCollectType::DATA_COLLECT_UNKNOW;
	bool									m_bPick = false;
	QString									m_DataCollectUid = "";
	AgriPestDataset							m_PestDataset;
	AgriPestPhotoIndeficationPtrVec			m_vecAssociteIndef;
	qnzkna::ZoneManage::CZoneBase			m_curZone;
	bool									m_bInitZone = false;
	AgriPestCollection						m_Pestcollect;
	QString									m_selectedImagePath = "";
	QLabel									*m_loadingLabel = nullptr;
	QMovie 									*m_loadingMovie = nullptr;
	recognize::IRecognizeInterface			*m_recognizeInter = nullptr;
private:
	Ui::AgriPestAndDiseaseDataCollectDialog *ui;
};
#endif  // AgriPestAndDiseaseDataCollectDialog_H