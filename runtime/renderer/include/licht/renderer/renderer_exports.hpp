#pragma once

#ifdef LICHT_RENDERER_EXPORTS
#ifdef _MSC_VER
#define LICHT_RENDERER_API __declspec(dllexport)
#else
#define LICHT_RENDERER_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_RENDERER_API __declspec(dllimport)
#else
#define LICHT_RENDERER_API
#endif
#endif