#ifndef STR_H
#define STR_H

#include <string>

typedef std::string Str;
typedef std::wstring WStr;

WStr swprintf(const wchar_t* format, ...);
WStr vswprintf(const wchar_t* format, va_list args);
WStr swprintf(const WStr& format, ...);
WStr vswprintf(const WStr& format, va_list args);
Str sprintf(const char* format, ...);
Str vsprintf(const char* format, va_list args);
Str sprintf(const Str& format, ...);
Str vsprintf(const Str& format, va_list args);

#endif