#ifndef __LOADER_H__
#define __LOADER_H__

enum ErrorCode
{
	ErrorCodeNone,
	ErrorCodeUnknown,
	ErrorCodeBadFormat,
	ErrorCodeNotFound
};

void* loadLibrary(char*, ErrorCode*);
bool unloadLibrary(void *handle);

#endif