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
	if(!(handle = LoadLibraryA(filepath)))
	{
		switch(GetLastError())
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

void* loadAddress(void *handle, char *exportedName, ErrorCode *error)
{
	void *ret = NULL;
	*error = ErrorCodeNone;

	if(!handle)
	{
		*error = ErrorCodeLibraryNotLoaded;
	}
	else
	{
#if defined(OS_WINDOWS)
		if(!(ret = GetProcAddress((HMODULE)handle, exportedName)))
		{
			switch(GetLastError())
			{
				case ERROR_PROC_NOT_FOUND:
					*error = ErrorCodeFunctionNotFound;
					break;
				default:
					*error = ErrorCodeUnknown;
					break;
			}
		}
#endif
	}
	return ret;
}