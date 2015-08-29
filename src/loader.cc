#include "os.h"
#include "loader.h"

#if defined(OS_WINDOWS)
#include <windows.h>
#endif

#include <stdio.h>
void* loadLibrary(char *filepath, ErrorCode *error)
{
	void *handle = NULL;
	*error = ErrorCodeNone;
#if defined(OS_WINDOWS)
	if((handle = LoadLibraryA(filepath)) == NULL)
	{
		unsigned long code = GetLastError();
		switch(code)
		{
			case ERROR_BAD_EXE_FORMAT:
				*error = ErrorCodeBadFormat;
				break;
			case ERROR_MOD_NOT_FOUND:
				*error = ErrorCodeNotFound;
				break;
			default:
				*error = ErrorCodeUnknown;
				break;
		}
	}
#endif
	return handle;
}

bool unloadLibrary(void *handle)
{
#if defined(OS_WINDOWS)
	return FreeLibrary((HMODULE)handle);
#endif
}