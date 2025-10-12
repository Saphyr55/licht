#pragma once

#ifdef LICHT_ENGINE_EXPORTS
#ifdef _MSC_VER
#define LICHT_ENGINE_API __declspec(dllexport)
#else
#define LICHT_ENGINE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_ENGINE_API __declspec(dllimport)
#else
#define LICHT_ENGINE_API
#endif
#endif