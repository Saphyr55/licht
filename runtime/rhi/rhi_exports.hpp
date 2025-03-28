#pragma once

#ifdef LICHT_RHI_EXPORTS
#ifdef _MSC_VER
#define LICHT_RHI_API __declspec(dllexport)
#else
#define LICHT_RHI_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_RHI_API __declspec(dllimport)
#else
#define LICHT_RHI_API 
#endif
#endif
