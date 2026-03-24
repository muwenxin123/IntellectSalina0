#include "AgriMultispectralSensorDecl.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AgriMSICameraStatusInfo>("AgriMSICameraStatusInfo")
	.constructor<>()
	.property("sd_gb_free", &AgriMSICameraStatusInfo::sd_gb_free)
	.property("sd_gb_total", &AgriMSICameraStatusInfo::sd_gb_total)
	.property("sd_type", &AgriMSICameraStatusInfo::sd_type)
	.property("sd_warn", &AgriMSICameraStatusInfo::sd_warn)
	.property("sd_status", &AgriMSICameraStatusInfo::sd_status)
	.property("bus_volts", &AgriMSICameraStatusInfo::bus_volts)
	.property("gps_used_sats", &AgriMSICameraStatusInfo::gps_used_sats)
	.property("gps_vis_sats", &AgriMSICameraStatusInfo::gps_vis_sats)
	.property("gps_warn", &AgriMSICameraStatusInfo::gps_warn)
	.property("gps_lat", &AgriMSICameraStatusInfo::gps_lat)
	.property("gps_lon", &AgriMSICameraStatusInfo::gps_lon)
	.property("gps_type", &AgriMSICameraStatusInfo::gps_type)
	.property("course", &AgriMSICameraStatusInfo::course)
	.property("alt_agl", &AgriMSICameraStatusInfo::alt_agl)
	.property("alt_msl", &AgriMSICameraStatusInfo::alt_msl)
	.property("p_acc", &AgriMSICameraStatusInfo::p_acc)
	.property("utc_time", &AgriMSICameraStatusInfo::utc_time)
	.property("vel_2d", &AgriMSICameraStatusInfo::vel_2d)
	.property("auto_cap_active", &AgriMSICameraStatusInfo::auto_cap_active)
	.property("dls_status", &AgriMSICameraStatusInfo::dls_status)
	.property("gps_time", &AgriMSICameraStatusInfo::gps_time)
	.property("utc_time_valid", &AgriMSICameraStatusInfo::utc_time_valid)
	.property("time_source", &AgriMSICameraStatusInfo::time_source)
	;
}