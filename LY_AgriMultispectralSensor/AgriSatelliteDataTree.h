#ifndef AGRISATELLITEDATATREE_H_
#define AGRISATELLITEDATATREE_H_
 
#include <QWidget>
#include <QGridLayout>
#include <QSqlQuery>
#include <QTreeWidget>
#include <QScrollArea>

#include "AgriMultispectralSensorDecl.h"

namespace Ui
{
	class AgriSatelliteDataTree;
}
 
class AgriSatelliteDataTree : public QWidget
{
	Q_OBJECT

public:
	struct ImageInfo {
		ImageInfo()
			:data("")
			, category("")
			, title("")
			, imagePath("")
			, description("")
		{};
		QString data;		  // 图像日期
		QString category;	  // 图像分类
		QString title;        // 图像名称
		QString imagePath;    // 图像路径
		QString description;  // 图像描述
	};
public:
	explicit AgriSatelliteDataTree(QWidget *parent = nullptr);
	~AgriSatelliteDataTree();

public:
	void Init();
	void InitTree();
	void CreateTree();

private:
	void BuildTree(QTreeWidget *treeWidge);
	void InsertLeafNode(QTreeWidget *treeWidget);
	void FilterTreeNodes(QTreeWidget *treeWidget, const QString &filter);
	bool FilterItemNodes(QTreeWidgetItem *item, const QString &filter);
	void DeleteTreeNode(QTreeWidgetItem *item);

signals:
	void LeafNodeClicked(QTreeWidgetItem *item, int column);
	void UpdateImageShow(QScrollArea *scrollArea);
	void UpdateCheckedImages(std::vector<ImageInfo> &imageVec);
	
private slots:
	void onItemClicked(QTreeWidgetItem *item, int column);
	void onItemChanged(QTreeWidgetItem *item, int column);
	void updateChildState(QTreeWidgetItem *item, Qt::CheckState state);
	void updateParentState(QTreeWidgetItem *item);
	void onTreeItemChanged(QTreeWidgetItem *item, int column);

private:
	Ui::AgriSatelliteDataTree									*ui;
	QWidget														*m_pContainerWidget;
	QGridLayout													*m_pGridLayout;
	QScrollArea													*m_pScrollArea;

	std::unordered_map<VegetationIndex, QString> m_mVegIndexMap = {
		{ VegetationIndex::VEG_INDEX_RGB, "RGB"},
		{ VegetationIndex::VEG_INDEX_NDVI, "NDVI" },
		{ VegetationIndex::VEG_INDEX_GNDVI, "GNDVI" },
		{ VegetationIndex::VEG_INDEX_NDRE, "NDRE" },
		{ VegetationIndex::VEG_INDEX_LCI, "LCI" },
		{ VegetationIndex::VEG_INDEX_RENDVI, "RENDVI" },
		{ VegetationIndex::VEG_INDEX_TVI, "TVI" },
		{ VegetationIndex::VEG_INDEX_DVI, "DVI" },
		{ VegetationIndex::VEG_INDEX_NDWI, "NDWI" },
		{ VegetationIndex::VEG_INDEX_GROWTH, "GROWTH" },
		{ VegetationIndex::VEG_INDEX_YIELD, "YIELD" }
	};
	std::unordered_map<VegetationIndex, QString> m_mVegIndexDescMap = {
		{ VegetationIndex::VEG_INDEX_RGB, tr2("RGB（可见光图像）") },
		{ VegetationIndex::VEG_INDEX_NDVI, tr2("NDVI（归一化植被指数）") },
		{ VegetationIndex::VEG_INDEX_GNDVI, tr2("GNDVI（绿光归一化植被指数）") },
		{ VegetationIndex::VEG_INDEX_NDRE, tr2("NDRE（归一化差值红边指数）") },
		{ VegetationIndex::VEG_INDEX_LCI, tr2("LCI（叶绿素指数）") },
		{ VegetationIndex::VEG_INDEX_RENDVI, tr2("RENDVI（红边归一化差异植被指数）") },
		{ VegetationIndex::VEG_INDEX_TVI, tr2("TVI（三角植被指数）") },
		{ VegetationIndex::VEG_INDEX_DVI, tr2("DVI（差异植被指数）") },
		{ VegetationIndex::VEG_INDEX_NDWI, tr2("NDWI（归一化水体指数）") },
		{ VegetationIndex::VEG_INDEX_GROWTH, tr2("GROWTH（作物长势图）") },
		{ VegetationIndex::VEG_INDEX_YIELD, tr2("YIELD（作物产量图）") }
	};

	QMap<QString, ImageInfo> m_mImageInfoMap;
};
#endif  // AgriSatelliteDataTree_H