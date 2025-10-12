#pragma once

#ifdef LICHT_SCENE_EXPORTS
#ifdef _MSC_VER
#define LICHT_SCENE_API __declspec(dllexport)
#else
#define LICHT_SCENE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_SCENE_API __declspec(dllimport)
#else
#define LICHT_SCENE_API
#endif
#endif