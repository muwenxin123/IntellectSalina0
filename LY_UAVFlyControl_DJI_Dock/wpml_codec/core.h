#pragma once

#ifndef WPML_CODEC_CORE_H
#define WPML_CODEC_CORE_H

#include <wpml_codec/common.h>
#include <wpml_codec/structs.h>
#include <boost/optional.hpp>

namespace wpml_codec
{
	namespace core
	{
		boost::optional<wcs::KMLDocument> LIB_API parseKML(const std::string& kmlPath);
		bool LIB_API createKML(const wcs::KMLDocument& data, const std::string& kmlPath);
		boost::optional<wcs::WPMLDocument> LIB_API parseWPML(const std::string& wpmlPath);
		bool LIB_API createWPML(const wcs::WPMLDocument& data, const std::string& wpmlPath);
		boost::optional<wcs::WPMZData> LIB_API parseWPMZ(const std::string& kmzPath);
		bool LIB_API createWPMZ(const wcs::WPMZData& data, const std::string& kmzPath);
	}
}; // namespace wpml_codec::core

namespace wcc = wpml_codec::core;

#endif // WPML_CODEC_CORE_H
