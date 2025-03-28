#pragma once

#ifdef LICHT_DISPLAY_EXPORTS
#ifdef _MSC_VER
#define LICHT_DISPLAY_API __declspec(dllexport)
#else
#define LICHT_DISPLAY_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_DISPLAY_API __declspec(dllimport)
#else
#define LICHT_DISPLAY_API 
#endif
#endif
