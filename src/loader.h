#ifndef __LOADER_H__
#define __LOADER_H__

enum ErrorCode
{
	ErrorCodeNone,
	ErrorCodeUnknown,
	ErrorCodeBadFormat,
	ErrorCodeNotFound,
	ErrorCodeLibraryNotLoaded,
	ErrorCodeFunctionNotFound
};

void* loadLibrary(char*, ErrorCode*);
bool unloadLibrary(void*);
void* loadAddress(void*, char*, ErrorCode*);

#endif