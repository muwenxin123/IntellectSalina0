#ifndef AGRIDATAANALYSISPREDICTDECL_H_
#define AGRIDATAANALYSISPREDICTDECL_H_
 
#include <QString>
#include <QVariant>


/* 气象数据 */
class AgriMetData
{
public:
	AgriMetData()
		: site(""),
		year(0),
		day(0),
		radn(0.0),
		maxt(0.0),
		mint(0.0),
		rain(0.0),
		evap(0.0)
	{};
	~AgriMetData() {};

	std::string				site;
	int						year;
	int						day;
	double					radn;
	double					maxt;
	double					mint;
	double					rain;
	double					evap;
};

typedef std::vector<AgriMetData>            AgriMetDataVec;
typedef AgriMetDataVec::iterator            AgriMetDataVecItr;
typedef std::vector<AgriMetData *>          AgriMetDataPtrVec;
typedef AgriMetDataPtrVec::iterator         AgriMetDataVecPtrVecItr;

Q_DECLARE_METATYPE(AgriMetData)

/* 输出数据 */
class AgriOutputData
{
public:
	AgriOutputData()
		: date(""),
		biomass(0.0),
		yield(0.0),
		grain_protein(0.0),
		grain_size(0.0),
		esw(0.0),
		lai(0.0),
		stage(0.0)
	{};
	~AgriOutputData() {};

	std::string				date;			// 日期，格式为日 / 月 / 年（dd / mm / yyyy）
	double					biomass;		// 生物量（kg / ha）
	double					yield;			// 产量（kg / ha）
	double					grain_protein;	// 谷物蛋白质含量（%）
	double					grain_size;		// 谷物粒径（g）
	double					esw;			// 土壤可提取水分（mm）
	double					lai;			// 叶面积指数（m ^ 2 / m ^ 2）
	double					stage;			// 作物生长阶段，通常为数值编码
};

typedef std::vector<AgriOutputData>            AgriOutputDataVec;
typedef AgriOutputDataVec::iterator            AgriOutputDataVecItr;
typedef std::vector<AgriOutputData *>          AgriOutputDataPtrVec;
typedef AgriOutputDataPtrVec::iterator         AgriOutputDataVecPtrVecItr;

Q_DECLARE_METATYPE(AgriOutputData)
#endif  // AgriDataAnalysisPredictDecl_H