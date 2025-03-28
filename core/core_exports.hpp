#pragma once

#ifdef LICHT_CORE_EXPORTS
#ifdef _MSC_VER
#define LICHT_CORE_API __declspec(dllexport)
#else
#define LICHT_CORE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_CORE_API __declspec(dllimport)
#else
#define LICHT_CORE_API
#endif
#endif