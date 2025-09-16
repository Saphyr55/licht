#pragma once

#ifdef LICHT_LAUNCHER_EXPORTS
#ifdef _MSC_VER
#define LICHT_LAUNCHER_API __declspec(dllexport)
#else
#define LICHT_LAUNCHER_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_LAUNCHER_API __declspec(dllimport)
#else
#define LICHT_LAUNCHER_API
#endif
#endif