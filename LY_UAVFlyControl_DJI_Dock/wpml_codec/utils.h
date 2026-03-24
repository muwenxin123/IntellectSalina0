#pragma once

#ifndef WPML_CODEC_UTILS_H
#define WPML_CODEC_UTILS_H

#include <wpml_codec/common.h>
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace wpml_codec{
	namespace utils
	{
		/**********************************************************************/
		/*                               Files                                */
		/**********************************************************************/

		bool LIB_API extractKMZ(const std::string& kmzPath, const std::string& outputDir);
		bool LIB_API packageKMZ(const std::string& inputDir, const std::string& kmzPath);
		std::vector<std::string> LIB_API findFiles(const std::string& directory);
		bool LIB_API copyFileOrDir(const std::string& path, const std::string& newPath);
		bool LIB_API removeFileOrDir(const std::string& path);
		bool LIB_API makeDir(const std::string& dir);
		std::string LIB_API getTempDir();
		std::string LIB_API getFileName(const std::string& filePath);

		/**********************************************************************/
		/*                               String                               */
		/**********************************************************************/

		std::string LIB_API toLower(const std::string& str);
		std::string LIB_API toUpper(const std::string& str);
		std::string LIB_API trim(const std::string& str);
		bool LIB_API equal(const std::string& str, const std::string& suffix, bool ignoreCase = false);
		bool LIB_API startWith(const std::string& str, const std::string& suffix, bool ignoreCase = false);
		bool LIB_API endWith(const std::string& str, const std::string& suffix, bool ignoreCase = false);
		std::vector<std::string> LIB_API split(const std::string& str, const std::string& separator);
		std::string LIB_API merge(const std::vector<std::string>& strVec, const std::string& separator = "");
		std::string LIB_API replace(const std::string& str, const std::string& oldSuffix, const std::string& newSuffix);
		boost::optional<bool> LIB_API toBool(const std::string& str);
		boost::optional<int> LIB_API toInt(const std::string& str);
		boost::optional<long long> LIB_API toLongLong(const std::string& str);
		boost::optional<double> LIB_API toDouble(const std::string& str, size_t decimal = DBL_DECIMAL);
		std::vector<double> LIB_API toDoubles(const std::string& str, size_t decimal = DBL_DECIMAL);
		std::string LIB_API toString(bool number);
		std::string LIB_API toString(int number);
		std::string LIB_API toString(long long number);
		std::string LIB_API toString(double number, size_t decimal = DBL_DECIMAL, bool clipZero = true);
		std::string LIB_API toString(std::vector<double> numbers, size_t decimal = DBL_DECIMAL, bool clipZero = true);
		std::string LIB_API escape(const std::string& str);
		std::string LIB_API removeEscape(const std::string& str);

		/**********************************************************************/
		/*                                Time                                */
		/**********************************************************************/

		std::string LIB_API getNowTimestamp();
	}
}; // namespace wpml_codec::utils

namespace wcu = wpml_codec::utils;

#endif // WPML_CODEC_UTILS_H
