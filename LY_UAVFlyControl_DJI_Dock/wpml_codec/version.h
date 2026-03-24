#pragma once

#ifndef WPML_CODEC_VERSION_H
#define WPML_CODEC_VERSION_H

#include <wpml_codec/common.h>
#include <iostream>
#include <boost/utility/string_view.hpp>


namespace wpml_codec
{
	namespace version
	{
		boost::string_view BRANCH_NAME = "@BRANCH_NAME@";
		boost::string_view COMMIT_HASH = "@COMMIT_HASH@";
		boost::string_view COMMIT_TIME = "@COMMIT_TIME@";
		boost::string_view BUILD_TIME = "@BUILD_TIME@";
		boost::string_view BUILD_ENV = "@BUILD_ENV@";
		boost::string_view BUILD_CXX_COMPILER = "@BUILD_CXX_COMPILER@";
		boost::string_view BUILD_C_COMPILER = "@BUILD_C_COMPILER@";
		boost::string_view RELEASE_VERSION = "@RELEASE_VERSION@";

		inline void LIB_API display()
		{
			std::cout << "BRANCH_NAME: " << BRANCH_NAME << "\n"
				<< "COMMIT_HASH: " << COMMIT_HASH << "\n"
				<< "COMMIT_TIME: " << COMMIT_TIME << "\n"
				<< "BUILD_CXX_COMPILER: " << BUILD_CXX_COMPILER << "\n"
				<< "BUILD_C_COMPILER: " << BUILD_C_COMPILER << "\n"
				<< "BUILD_ENV: " << BUILD_ENV << "\n"
				<< "BUILD_TIME: " << BUILD_TIME << "\n"
				<< "RELEASE_VERSION: " << RELEASE_VERSION << "\n"
				<< std::endl;
		}
	}
}; // namespace wpml_codec::version

namespace wcv = wpml_codec::version;

#endif // WPML_CODEC_VERSION_H
