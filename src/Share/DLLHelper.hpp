#pragma once

#ifdef _WIN32
#include <wtypes.h>
typedef HMODULE		DllHandle;
typedef void*		ProcHandle;
#else
#include <dlfcn.h>
typedef void*		DllHandle;
typedef void*		ProcHandle;
#endif

class DLLHelper
{
public:
	static DllHandle load_library(const char *filename)
	{
#ifdef _WIN32
		return ::LoadLibrary(filename);
#else
		DllHandle ret = dlopen(filename, RTLD_NOW);
		if(ret == NULL)
			printf("%s\n", dlerror());
		return ret;
#endif
	}

	static void free_library(DllHandle handle)
	{
#ifdef _WIN32
		::FreeLibrary(handle);
#else
		dlclose(handle);
#endif
	}

	static ProcHandle get_symbol(DllHandle handle, const char* name)
	{
#ifdef _WIN32
		return ::GetProcAddress(handle, name);
#else
		return dlsym(handle, name);
#endif
	}
};