#ifndef __LNKLIB_H__
#define __LNKLIB_H__

enum LibraryHandleType
{
	LibraryHandleTypeModule,
	LibraryHandleTypeFunction
};

struct LibraryHandleStore
{
	long Id;
	void *Handle;
	LibraryHandleType Type;
};

void load(const v8::FunctionCallbackInfo<v8::Value>&);
void unload(const v8::FunctionCallbackInfo<v8::Value>&);
void getFunction(const v8::FunctionCallbackInfo<v8::Value>&);
void clearFunction(const v8::FunctionCallbackInfo<v8::Value>&);

void* getHandle(long, LibraryHandleType);
long addHandle(void*, LibraryHandleType);
bool removeHandle(long, LibraryHandleType);

#endif