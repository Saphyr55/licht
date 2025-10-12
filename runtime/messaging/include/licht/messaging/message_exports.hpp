#pragma once

#ifdef LICHT_MESSAGING_EXPORTS
#ifdef _MSC_VER
#define LICHT_MESSAGING_API __declspec(dllexport)
#else
#define LICHT_MESSAGING_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_MESSAGING_API __declspec(dllimport)
#else
#define LICHT_MESSAGING_API 
#endif
#endif
