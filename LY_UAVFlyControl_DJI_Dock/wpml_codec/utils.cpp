#include <wpml_codec/utils.h>
#include <ghc/filesystem.hpp>
#include <zip/unzip.h>
#include <zip/zip.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace chrono = std::chrono;
namespace fs = ghc::filesystem;

namespace wpml_codec
{
	namespace utils
	{
		/**********************************************************************/
		/*                               Files                                */
		/**********************************************************************/

		bool extractKMZ(const std::string& kmzPath, const std::string& outputDir)
		{
			unzFile zipFile = unzOpen(kmzPath.c_str());
			if (!zipFile)
			{
				return false;
			}
			if (unzGoToFirstFile(zipFile) != UNZ_OK)
			{
				unzClose(zipFile);
				return false;
			}
			do
			{
				constexpr int FILE_NAME_LENGTH = 256;
				char fileNameInZip[FILE_NAME_LENGTH];
				unz_file_info fileInfo;
				if (unzGetCurrentFileInfo(zipFile, &fileInfo, fileNameInZip, FILE_NAME_LENGTH, nullptr, 0, nullptr, 0) !=
					UNZ_OK)
				{
					unzClose(zipFile);
					return false;
				}
				std::string entryName(fileNameInZip);
				fs::path outputPath = fs::path(outputDir) / entryName;
				fs::create_directories(outputPath.parent_path());
				FILE *outFile = fopen(outputPath.string().c_str(), "wb");
				if (!outFile)
				{
					unzClose(zipFile);
					return false;
				}
				if (unzOpenCurrentFile(zipFile) != UNZ_OK)
				{
					fclose(outFile);
					unzClose(zipFile);
					return false;
				}
				char buffer[4096];
				int bytesRead;
				bool error = false;
				while (bytesRead = unzReadCurrentFile(zipFile, buffer, sizeof(buffer)))
				{
					if (bytesRead < 0)
					{
						error = true;
						break;
					}
					if (fwrite(buffer, 1, bytesRead, outFile) != bytesRead)
					{
						error = true;
						break;
					}
				}
				fclose(outFile);
				unzCloseCurrentFile(zipFile);
				if (error)
				{
					fs::remove(outputPath);
					unzClose(zipFile);
					return false;
				}

			} while (unzGoToNextFile(zipFile) == UNZ_OK);
			unzClose(zipFile);
			return true;
		}

		bool packageKMZ(const std::string& inputDir, const std::string& kmzPath)
		{
			zipFile zipFile = zipOpen(kmzPath.c_str(), APPEND_STATUS_CREATE);
			if (!zipFile)
			{
				return false;
			}
			try
			{
				for (const auto& entry : fs::recursive_directory_iterator(inputDir))
				{
					if (entry.is_regular_file())
					{
						fs::path relativePath = fs::relative(entry.path(), inputDir);
						std::string zipEntryName = relativePath.generic_string();
						FILE *inFile = fopen(entry.path().string().c_str(), "rb");
						if (!inFile)
						{
							zipClose(zipFile, nullptr);
							return false;
						}
						fseek(inFile, 0, SEEK_END);
						// long fileSize = ftell(inFile);
						fseek(inFile, 0, SEEK_SET);
						zip_fileinfo fileInfo = {};
						if (zipOpenNewFileInZip(zipFile,
							zipEntryName.c_str(),
							&fileInfo,
							nullptr,
							0,
							nullptr,
							0,
							nullptr,
							Z_DEFLATED,
							Z_DEFAULT_COMPRESSION) != ZIP_OK)
						{
							fclose(inFile);
							zipClose(zipFile, nullptr);
							return false;
						}
						char buffer[4096];
						size_t bytesRead;
						bool error = false;
						while (bytesRead = fread(buffer, 1, sizeof(buffer), inFile))
						{
							if (zipWriteInFileInZip(zipFile, buffer, (unsigned int)bytesRead) != ZIP_OK)
							{
								error = true;
								break;
							}
						}
						fclose(inFile);
						zipCloseFileInZip(zipFile);

						if (error)
						{
							zipClose(zipFile, nullptr);
							return false;
						}
					}
				}
			}
			catch (const std::exception&)
			{
				zipClose(zipFile, nullptr);
				return false;
			}
			if (zipClose(zipFile, nullptr) != ZIP_OK)
			{
				return false;
			}
			return true;
		}

		std::vector<std::string> findFiles(const std::string& directory)
		{
			std::vector<std::string> files{};
			if (!fs::exists(directory) || !fs::is_directory(directory))
			{
				return files;
			}
			for (const auto& fit : fs::recursive_directory_iterator(directory))
			{
				if (fit.is_regular_file())
				{
					files.emplace_back(fs::absolute(fit.path()).lexically_normal().string());
				}
			}
			return files;
		}

		bool copyFileOrDir(const std::string& path, const std::string& newPath)
		{
			if (!fs::exists(path))
			{
				return false;
			}
			const auto copyOptions = fs::copy_options::update_existing | fs::copy_options::recursive;
			if (fs::is_directory(path))
			{
				try
				{
					fs::copy(path, newPath, copyOptions);
					return true;
				}
				catch (const fs::filesystem_error& e)
				{
					std::cerr << "[" << e.code().message() << "]: " << e.what() << "\n";
					return false;
				}
			}
			else if (fs::is_regular_file(path))
			{
				return fs::copy_file(path, newPath, copyOptions);
			}
			return false;
		}

		bool removeFileOrDir(const std::string& path)
		{
			if (fs::exists(path))
			{
				try
				{
					fs::remove_all(path);
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			return false;
		}

		bool makeDir(const std::string& dir)
		{
			return fs::create_directories(dir);
		}

		std::string getTempDir()
		{
			return fs::temp_directory_path().string();
		}

		std::string getFileName(const std::string& filePath)
		{
			fs::path path(filePath);
			return path.has_filename() ? path.filename().string() : "";
		}

		/**********************************************************************/
		/*                               String                               */
		/**********************************************************************/

		std::string toLower(const std::string& str)
		{
			std::string s(str);
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
			return s;
		}

		std::string toUpper(const std::string& str)
		{
			std::string s(str);
			std::transform(s.begin(), s.end(), s.begin(), ::toupper);
			return s;
		}

		std::string trim(const std::string& str)
		{
			std::string s(str);
			size_t finish = s.length() - 1;
			while (finish >= 0 && std::isspace(str[finish]))
			{
				--finish;
			}
			size_t start = 0;
			while (start <= finish && std::isspace(str[start]))
			{
				++start;
			}
			return str.substr(start, finish - start + 1);
		}

		bool equal(const std::string& str, const std::string& suffix, bool ignoreCase)
		{
			size_t srcLen = str.size();
			size_t desLen = suffix.size();
			if (srcLen != desLen)
			{
				return false;
			}
			if (ignoreCase)
			{
				return toLower(str) == toLower(suffix);
			}
			return str == suffix;
		}

		bool startWith(const std::string& str, const std::string& suffix, bool ignoreCase)
		{
			size_t srcLen = str.size();
			size_t startLen = suffix.size();
			if (srcLen < startLen)
			{
				return false;
			}
			std::string temp = str.substr(0, startLen);
			if (ignoreCase)
			{
				return toLower(temp) == toLower(suffix);
			}
			return temp == suffix;
		}

		bool endWith(const std::string& str, const std::string& suffix, bool ignoreCase)
		{
			size_t srcLen = str.size();
			size_t endLen = suffix.size();
			if (srcLen < endLen)
			{
				return false;
			}
			std::string temp = str.substr(srcLen - endLen, endLen);
			if (ignoreCase)
			{
				return toUpper(temp) == toUpper(suffix);
			}
			return temp == suffix;
		}

		std::vector<std::string> split(const std::string& str, const std::string& separator)
		{
			std::vector<std::string> res;
			if (separator.empty())
			{
				res.push_back(str);
				return res;
			}
			size_t lastPos = 0;
			size_t pos = str.find(separator, lastPos);
			while (pos != std::string::npos)
			{
				res.emplace_back(str.substr(lastPos, pos - lastPos));
				lastPos = pos + separator.size();
				pos = str.find(separator, lastPos);
			}
			res.emplace_back(str.substr(lastPos));
			return res;
		}

		std::string merge(const std::vector<std::string>& strVec, const std::string& separator)
		{
			std::string res = "";
			size_t strLen = strVec.size();
			for (int i = 0; i < strLen - 1; ++i)
			{
				res += (strVec[i] + separator);
			}
			res += strVec[strLen - 1];
			return res;
		}

		std::string replace(const std::string& str, const std::string& oldSuffix, const std::string& newSuffix)
		{
			if (oldSuffix.empty())
			{
				return str;
			}
			std::string result;
			size_t oldLen = oldSuffix.size();
			size_t newLen = newSuffix.size();
			size_t count = 0;
			size_t pos = 0;
			while ((pos = str.find(oldSuffix, pos)) != std::string::npos)
			{
				++count;
				pos += oldLen;
			}
			result.reserve(str.size() + count * (newLen - oldLen));
			pos = 0;
			size_t lastPos = 0;
			while ((pos = str.find(oldSuffix, lastPos)) != std::string::npos)
			{
				result.append(str, lastPos, pos - lastPos);
				result.append(newSuffix);
				lastPos = pos + oldLen;
			}
			result.append(str, lastPos);
			return result;
		}

		boost::optional<bool> toBool(const std::string& str)
		{
			if (equal(str, "true", true))
			{
				return true;
			}
			else if (equal(str, "false", true))
			{
				return false;
			}
			return boost::none;
		}

		boost::optional<int> toInt(const std::string& str)
		{
			std::istringstream iss(str);
			int value;
			iss >> value;
			if (iss.fail() || !iss.eof())
			{
				return boost::none;
			}
			return value;
		}

		boost::optional<long long> toLongLong(const std::string& str)
		{
			std::istringstream iss(str);
			long long value;
			iss >> value;
			if (iss.fail() || !iss.eof())
			{
				return boost::none;
			}
			return value;
		}

		boost::optional<double> toDouble(const std::string& str, size_t decimal)
		{
			std::istringstream iss(str);
			double value;
			iss >> std::setprecision(decimal) >> std::fixed >> value;
			if (iss.fail() || !iss.eof())
			{
				return boost::none;
			}
			return value;
		}

		std::vector<double> toDoubles(const std::string& str, size_t decimal)
		{
			std::vector<double> res{};
			std::vector<std::string> numStrs = split(str, ",");
			for (const std::string& numStr : numStrs)
			{
				auto tmp = toDouble(numStr, decimal);
				if (tmp.is_initialized())
				{
					res.emplace_back(tmp.value());
				}
			}
			return res;
		}

		std::string toString(bool number)
		{
			return number ? "true" : "false";
		}

		std::string toString(int number)
		{
			return std::to_string(number);
		}

		std::string toString(long long number)
		{
			return std::to_string(number);
		}

		std::string toString(double number, size_t decimal, bool clipZero)
		{
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(decimal) << number;
			std::string res = oss.str();
			if (clipZero)
			{
				size_t posZero = res.find_last_not_of('0');
				if (posZero == std::string::npos)
				{
					return res;
				}
				size_t posDot = res.rfind('.');
				if (posDot == std::string::npos)
				{
					return res;
				}
				if (posZero > posDot)
				{
					res.erase(posZero + 1);
				}
				else if (posZero == posDot)
				{
					res.erase(posDot);
				}
			}
			return res;
		}

		std::string toString(std::vector<double> numbers, size_t decimal, bool clipZero)
		{
			std::string res;
			for (double number : numbers)
			{
				res += toString(number, decimal, clipZero) + ",";
			}
			if (endWith(res, ","))
			{
				res.pop_back(); // Remove the last symbol
			}
			return res;
		}

		std::string escape(const std::string& str)
		{
			std::string result;
			result.reserve(str.size() * 2);
			for (char c : str)
			{
				// Chinese characters are directly retained (UTF-8 encoded bytes > 0x7F)
				if (static_cast<unsigned char>(c) >= 0x80)
				{
					result += c;
					continue;
				}
				// Handle JSON standard escape
				switch (c)
				{
				case '\"':
					result += "\\\"";
					break;
				case '\\':
					result += "\\\\";
					break;
				case '/':
					result += "\\/";
					break;
				case '\b':
					result += "\\b";
					break;
				case '\f':
					result += "\\f";
					break;
				case '\n':
					result += "\\n";
					break;
				case '\r':
					result += "\\r";
					break;
				case '\t':
					result += "\\t";
					break;
				default:
					if (c < 0x20)
					{
						// The control character is escaped as \uXXXX
						char buf[7];
						snprintf(buf, sizeof(buf), "\\u%04X", static_cast<unsigned char>(c));
						result += buf;
					}
					else
					{
						result += c;
					}
					break;
				}
			}
			return result;
		}

		std::string removeEscape(const std::string& str)
		{
			std::string result;
			result.reserve(str.size());
			bool escape = false;
			for (size_t i = 0; i < str.size(); ++i)
			{
				const char c = str[i];
				if (escape)
				{
					switch (c)
					{
					case '"':
						result += '"';
						break;
					case '\\':
						result += '\\';
						break;
					case '/':
						result += '/';
						break;
					case 'b':
						result += '\b';
						break;
					case 'f':
						result += '\f';
						break;
					case 'n':
						result += '\n';
						break;
					case 'r':
						result += '\r';
						break;
					case 't':
						result += '\t';
						break;
					case 'u':
					{
						// Unicode
						if (i + 4 >= str.size())
						{
							// Illegal escape. Keep as is
							result += "\\u";
							break;
						}
						// Extract a 4-digit hexadecimal value
						uint16_t code = 0;
						for (int j = 0; j < 4; ++j)
						{
							code <<= 4;
							char hex = str[++i];
							if (hex >= '0' && hex <= '9')
							{
								code |= hex - '0';
							}
							else if (hex >= 'A' && hex <= 'F')
							{
								code |= hex - 'A' + 10;
							}
							else if (hex >= 'a' && hex <= 'f')
							{
								code |= hex - 'a' + 10;
							}
						}
						// Convert to UTF-8 encoding
						if (code <= 0x7F)
						{
							result += static_cast<char>(code);
						}
						else if (code <= 0x7FF)
						{
							result += static_cast<char>(0xC0 | (code >> 6));
							result += static_cast<char>(0x80 | (code & 0x3F));
						}
						else
						{
							result += static_cast<char>(0xE0 | (code >> 12));
							result += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
							result += static_cast<char>(0x80 | (code & 0x3F));
						}
						break;
					}
					default: // Illegal escape. Keep as is
						result += '\\';
						result += c;
						break;
					}
					escape = false;
				}
				else
				{
					if (c == '\\')
					{
						escape = true;
					}
					else
					{
						result += c;
					}
				}
			}
			if (escape)
			{
				result += '\\'; // The unclosed backslash at the end
			}
			return result;
		}

		std::string getNowTimestamp()
		{
			auto ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
			return toString(static_cast<long long>(ms.count()));
		}
	}
} // namespace wpml_codec::utils
