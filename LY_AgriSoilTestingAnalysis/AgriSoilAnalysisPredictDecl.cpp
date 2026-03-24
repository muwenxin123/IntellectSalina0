#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

using namespace AgriSoilTestingFertilizer;

RTTR_REGISTRATION
{
	rttr::registration::class_<AgriSoilTestingCollection>("AgriSoilTestingCollection")
		.constructor<>()
		.property("guid", &AgriSoilTestingCollection::guid)
		.property("name", &AgriSoilTestingCollection::name)
		.property("dataset_id", &AgriSoilTestingCollection::dataset_id)
		.property("sample_type", &AgriSoilTestingCollection::sample_type)
		.property("sample_mode", &AgriSoilTestingCollection::sample_mode)
		.property("sample_time", &AgriSoilTestingCollection::sample_time)
		.property("lon", &AgriSoilTestingCollection::lon)
		.property("lat", &AgriSoilTestingCollection::lat)
		.property("alt", &AgriSoilTestingCollection::alt)

		// !!! type 有效索引类型不可随意修改或添加，对应 valid_identify 不同标识，需统一修改

		/* ------------------- 养分元素 ------------------- */

		.property("ele_nutrient_N", &AgriSoilTestingCollection::ele_nutrient_N)
			(rttr::metadata("display", "氮"),
				rttr::metadata("type", "ele_nutrient"))
		.property("ele_nutrient_P", &AgriSoilTestingCollection::ele_nutrient_P)
			(rttr::metadata("display", "磷"),
				rttr::metadata("type", "ele_nutrient"))
		.property("ele_nutrient_K", &AgriSoilTestingCollection::ele_nutrient_K)
			(rttr::metadata("display", "钾"),
				rttr::metadata("type", "ele_nutrient"))

		/* ------------------- 微量元素 ------------------- */

		.property("ele_minor_Ca", &AgriSoilTestingCollection::ele_minor_Ca)
			(rttr::metadata("display", "钙"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Mg", &AgriSoilTestingCollection::ele_minor_Mg)
			(rttr::metadata("display", "镁"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Cu", &AgriSoilTestingCollection::ele_minor_Cu)
			(rttr::metadata("display", "铜"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Zn", &AgriSoilTestingCollection::ele_minor_Zn)
			(rttr::metadata("display", "锌"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Fe", &AgriSoilTestingCollection::ele_minor_Fe)
			(rttr::metadata("display", "铁"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_S", &AgriSoilTestingCollection::ele_minor_S)
			(rttr::metadata("display", "硫"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Mn", &AgriSoilTestingCollection::ele_minor_Mn)
			(rttr::metadata("display", "锰"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Mo", &AgriSoilTestingCollection::ele_minor_Mo)
			(rttr::metadata("display", "钼"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_B", &AgriSoilTestingCollection::ele_minor_B)
			(rttr::metadata("display", "硼"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Cl", &AgriSoilTestingCollection::ele_minor_Cl)
			(rttr::metadata("display", "氯"),
				rttr::metadata("type", "ele_minor"))
		.property("ele_minor_Si", &AgriSoilTestingCollection::ele_minor_Si)
			(rttr::metadata("display", "硅"),
				rttr::metadata("type", "ele_minor"))

		/* ---------------- 重金属污染元素 ---------------- */

		.property("ele_heavy_metal_Pb", &AgriSoilTestingCollection::ele_heavy_metal_Pb)
			(rttr::metadata("display", "铅"),
				rttr::metadata("type", "ele_heavy_metal"))
		.property("ele_heavy_metal_Cr", &AgriSoilTestingCollection::ele_heavy_metal_Cr)
			(rttr::metadata("display", "铬"),
				rttr::metadata("type", "ele_heavy_metal"))
		.property("ele_heavy_metal_Cd", &AgriSoilTestingCollection::ele_heavy_metal_Cd)
			(rttr::metadata("display", "镉"),
				rttr::metadata("type", "ele_heavy_metal"))
		.property("ele_heavy_metal_Hg", &AgriSoilTestingCollection::ele_heavy_metal_Hg)
			(rttr::metadata("display", "汞"),
				rttr::metadata("type", "ele_heavy_metal"))

		/* --------------- 土壤有机质含量 % --------------- */

		.property("organic_matter", &AgriSoilTestingCollection::organic_matter)
			(rttr::metadata("display", "有机质"),
				rttr::metadata("type", "organic_matter"))

		/* ------------------- 土壤墒情 ------------------- */

		.property("mois_Temp", &AgriSoilTestingCollection::mois_Temp)
			(rttr::metadata("display", "土壤温度特性"),
				rttr::metadata("type", "mois"))
		.property("mois_Humid", &AgriSoilTestingCollection::mois_Humid)
			(rttr::metadata("display", "土壤湿度特性"),
				rttr::metadata("type", "mois"))
		.property("mois_PH", &AgriSoilTestingCollection::mois_PH)
			(rttr::metadata("display", "土壤酸碱度"),
				rttr::metadata("type", "mois"))
		.property("mois_EC", &AgriSoilTestingCollection::mois_EC)
			(rttr::metadata("display", "土壤电导率"),
				rttr::metadata("type", "mois"))
		.property("mois_Water", &AgriSoilTestingCollection::mois_Water)
			(rttr::metadata("display", "土壤水势"),
				rttr::metadata("type", "mois"))
		;
}