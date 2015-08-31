#ifndef __CALLABLE_H__
#define __CALLABLE_H__

#include "os.h"

typedef void*(*dynamic_exec)(void);

struct dynamic_func
{
	void *address;
	char **params;
	size_t count;
	bool returns;
	dynamic_exec exec;
	size_t execSize;
};

struct dynamic_func* init_dynamic_func(void*, bool);
void dynamic_func_free(struct dynamic_func*);
void dynamic_func_add_param(struct dynamic_func*, char**);
dynamic_exec dynamic_func_compile(struct dynamic_func*);

#endif