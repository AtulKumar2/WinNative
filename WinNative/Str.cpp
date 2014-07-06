#include "Str.h"
#include <stdio.h>
#include <stdarg.h>

WStr swprintf(const wchar_t* format, ...)
{ 
	va_list args; 
	va_start(args, format); 
	WStr result = vswprintf(format, args); 
	va_end(args); 
	return result;
}

WStr vswprintf(const wchar_t* format, va_list args)
{ 
	const int bufferSize = 16384; 
	wchar_t buffer[bufferSize]; 
	vswprintf(buffer, bufferSize, format, args); 
	return WStr(buffer);
}

WStr swprintf(const WStr& format, ...)
{ 
	va_list args; 
	va_start(args, format); 
	WStr result = vswprintf(format, args); 
	va_end(args); 
	return result;
}

WStr vswprintf(const WStr& format, va_list args)
{ 
	return vswprintf(format.c_str(), args);
}

Str sprintf(const char* format, ...)
{ 
	va_list args; 
	va_start(args, format); 
	Str result = vsprintf(format, args); 
	va_end(args); 
	return result;
}

Str vsprintf(const char* format, va_list args)
{ 
	const int bufferSize = 16384; 
	char buffer[bufferSize]; 
	vsnprintf(buffer, bufferSize, format, args); 
	return Str(buffer);
}

Str sprintf(const Str& format, ...)
{ 
	va_list args; 
	va_start(args, format); 
	Str result = vsprintf(format, args); 
	va_end(args); 
	return result;
}

Str vsprintf(const Str& format, va_list args)
{ 
	return vsprintf(format.c_str(), args);
}

// This will crash the application eventually
int StringArray()
{
	const char *s[] = { "a", "b", "c", NULL };
	const char **p = s;
	while (*p != NULL) 
	{
		printf("string = %s hex = %x\n", *p, *p);
		printf("pointer = %x\n", p);
		(*p)++;
	}
	return 0;
}

int StrTest()
{
	//StringArray();
	return 0;
}