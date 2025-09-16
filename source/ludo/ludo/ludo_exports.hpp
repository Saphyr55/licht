#pragma once

#ifdef LICHT_LUDO_EXPORTS
#ifdef _MSC_VER
#define LICHT_LUDO_API __declspec(dllexport)
#else
#define LICHT_LUDO_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_LUDO_API __declspec(dllimport)
#else
#define LICHT_LUDO_API
#endif
#endif