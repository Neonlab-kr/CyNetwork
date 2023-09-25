/*
	Note: This source code is written by beginners and can be terrible.
*/

#include "file.h"

#include <io.h>
#include <stdio.h>

int exists(const char* path)
{
	return _access(path, 0) == 0;
}

int isDirExists(const char* path) {
	_finddatai64_t c_file;
	intptr_t hFile;
	int result = 0;

	hFile = _findfirsti64(path, &c_file);
	if (c_file.attrib & _A_SUBDIR) result = 1;
	_findclose(hFile);

	return result;
}