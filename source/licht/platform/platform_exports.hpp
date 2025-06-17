#pragma once

#ifdef LICHT_PLATFORM_EXPORTS
#ifdef _MSC_VER
#define LICHT_PLATFORM_API __declspec(dllexport)
#else
#define LICHT_PLATFORM_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_PLATFORM_API __declspec(dllimport)
#else
#define LICHT_PLATFORM_API 
#endif
#endif
