#ifndef MD5_H
#define MD5_H

#ifdef __cplusplus
extern "C" {
#endif

	void md5_data(const void* data, unsigned int size, char* result);
	void md5_file(const char* file_name, char* result);

#ifdef __cplusplus
}
#endif

#endif