#include <node.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "lnklib.h"
#include "loader.h"

#include <stdio.h>

void load(const v8::FunctionCallbackInfo<v8::Value>&);
void unload(const v8::FunctionCallbackInfo<v8::Value>&);

void init(v8::Handle<v8::Object> exports)
{
	NODE_SET_METHOD(exports, "load", load);
    NODE_SET_METHOD(exports, "unload", unload);
}

char* getV8String(v8::Local<v8::Value> value)
{
	char *ret = NULL;
    if (value->IsString())
	{
        v8::String::Utf8Value utf8(value);
        ret = (char*) malloc(sizeof(char) * utf8.length() + 1);
        strcpy(ret, *utf8);
    }
	return ret;
}

void throwExceptionForErrorCode(v8::Isolate* isolate, ErrorCode error)
{
	char *errorMsg = NULL;
	switch(error)
	{
		case ErrorCodeBadFormat:
			errorMsg = "Library is an invalid executable format";
			break;
        case ErrorCodeNotFound:
			errorMsg = "Could not find library path";
			break;
		default:
			errorMsg = "An error has occurred";
			break;
	}
	
	if(errorMsg != NULL)
	{
		isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, errorMsg)));
	}
}

struct LibraryHandleStore *libraryHandles = NULL;
size_t libraryHandleCount = 0;
long libraryLastId = 0;
    
void* getHandle(long id)
{
    void *ret = NULL;
    if(libraryHandleCount > 0 && libraryHandles != NULL)
    {
        LibraryHandleStore *lptr = libraryHandles;
        for(size_t i = 0; i < libraryHandleCount; i++, lptr++)
        {
            if(id == lptr->Id)
            {
                ret = lptr->Handle;
                break;
            }
        }
    }
    return ret;
}

long addHandle(void *handle)
{
    if(libraryHandles == NULL)
    {
        libraryHandles = (struct LibraryHandleStore*)malloc(sizeof(struct LibraryHandleStore));
    }
    else
    {
        libraryHandles = (struct LibraryHandleStore*)realloc(libraryHandles, sizeof(struct LibraryHandleStore) * (libraryHandleCount + 1));
    }
    
    libraryHandles[libraryHandleCount].Id = ++libraryLastId;
    libraryHandles[libraryHandleCount].Handle = handle;
    
    return libraryHandles[libraryHandleCount++].Id;
}

bool removeHandle(long id)
{
    if(libraryHandleCount > 0 && libraryHandles != NULL)
    {
        size_t i = 0;
        
        LibraryHandleStore *lptr = libraryHandles;
        for(; i < libraryHandleCount; i++, lptr++)
        {
            if(id == lptr->Id)
            {
                break;
            }
        }
        
        if(i < libraryHandleCount)
        {
            size_t newSize = libraryHandleCount - 1;
            for(; i < newSize; i++)
            {
                libraryHandles[i] = libraryHandles[i + 1];
            }
            
            libraryHandleCount = newSize;
            libraryHandles = (struct LibraryHandleStore*)realloc(libraryHandles, sizeof(struct LibraryHandleStore) * libraryHandleCount);
            
            return true;
        }
    }
    
    return false;
}

void load(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);
    
    if(args.Length() > 0)
    {
        if(args[0]->IsString())
        {
            char *libraryPath = getV8String(args[0]);
            
            ErrorCode error;
            void *handle = loadLibrary(libraryPath, &error);
            
            if(error != ErrorCodeNone)
            {
                if(handle)
                {
                    unloadLibrary(handle);
                }
                throwExceptionForErrorCode(isolate, error);
            }
            else
            {
                args.GetReturnValue().Set(v8::Integer::New(isolate, addHandle(handle)));
            }
        }
        else
        {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Invalid parameter type, must be a string")));
        }
    }
    else
    {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Must supply a library path")));
    }
}

void unload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
	v8::HandleScope scope(isolate);
    
    if(args.Length() > 0)
    {
        if(args[0]->IsInt32())
        {
            long handleId = (long)v8::Handle<v8::Integer>::Cast(args[0])->Value();
            args.GetReturnValue().Set(v8::Boolean::New(isolate, unloadLibrary(getHandle(handleId)) && removeHandle(handleId)));
        }
        else
        {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Invalid parameter type, must be an integer")));
        }
    }
    else
    {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Must supply a library handle ID")));
    }
}

NODE_MODULE(addon, init)