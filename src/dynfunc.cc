#include <stdlib.h>
#include <memory.h>
#include "dynfunc.h"

#if defined(OS_WINDOWS)
#include <Windows.h>
#endif

struct dynamic_func* init_dynamic_func(void *function, bool returns)
{
	struct dynamic_func *ret = (struct dynamic_func*)malloc(sizeof(struct dynamic_func));
	memset(ret, 0, sizeof(struct dynamic_func));
	ret->address = function;
	ret->returns = returns;
	return ret;
}

void dynamic_func_free(struct dynamic_func *df)
{
	if (df->params)
	{
		free(df->params);
	}

	if (df->exec)
	{
#if defined(OS_WINDOWS)
		VirtualFree(df->exec, NULL, MEM_RELEASE);
#endif
	}

	free(df);
}

void dynamic_func_add_param(struct dynamic_func *df, char **buf)
{
	if (!df->params)
	{
		df->params = (char**)malloc(sizeof(void**));
	}
	else
	{
		df->params = (char**)realloc(df->params, sizeof(char**) * (df->count + 1));
	}
	*(df->params + df->count++) = *buf;
}

dynamic_exec dynamic_func_compile(struct dynamic_func *df)
{
	if (df->exec)
	{
		return df->exec;
	}

	unsigned char *buffer = NULL;

#if defined(ARCH_X64)
#define _X64OVERHEAD(n) (n)
#else
#define _X64OVERHEAD(n) 0
#endif

	df->execSize = sizeof(void*) + 4 + _X64OVERHEAD(1); // Base JIT size, mov, call, and ret
	if (df->params)
	{
		df->execSize += df->count * (sizeof(void*) + 3 + _X64OVERHEAD(1)); // mov, push, and pop
		if (df->returns)
		{
			df->execSize += 4 + _X64OVERHEAD(2); // double mov operations
		}
	}

#undef _X64OVERHEAD

#if defined(OS_WINDOWS)
	buffer = (unsigned char*)VirtualAlloc(0, df->execSize, MEM_COMMIT, PAGE_READWRITE);
#endif

	if (buffer)
	{
		unsigned char *p = buffer;

#if defined(ARCH_X64)
#define _X64PREFIX() *p++ = 0x48;
#else
#define _X64PREFIX()
#endif

		if (df->params)
		{
			for (size_t i = df->count - 1; i != -1; i--)
			{
				_X64PREFIX(); *p++ = 0xB8; (void*&)p[0] = (void*)df->params[i]; p += sizeof(void*); // mov eax, df->params[i]
				*p++ = 0x50; // push eax
			}
		}

		_X64PREFIX(); *p++ = 0xB8; (void*&)p[0] = (void*)df->address; p += sizeof(void*); // mov eax, df->address
		*p++ = 0xFF; *p++ = ((0x3 & 0x3) << 6) | ((0x7 & 0x2) << 3) | (0x7 & 0x0); // call eax

		if (df->returns)
		{
			_X64PREFIX(); *p++ = 0x89; *p++ = ((0x3 & 0x3) << 6) | ((0x7 & 0x0) << 3) | (0x7 & 0x1); // mov ecx, eax
		}

		for (size_t i = 0; i < df->count; i++)
		{
			*p++ = 0x58; // pop eax
		}

		if (df->returns)
		{
			_X64PREFIX(); *p++ = 0x89; *p++ = ((0x3 & 0x3) << 6) | ((0x7 & 0x1) << 3) | (0x7 & 0x0); // mov eax, ecx
		}

		*p++ = 0xC3; // ret
	}

#undef _X64PREFIX

#if defined(OS_WINDOWS)
	unsigned long old = NULL;
	VirtualProtect(buffer, df->execSize, PAGE_EXECUTE, &old);
#endif

	return (dynamic_exec)buffer;
}