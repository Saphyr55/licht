#pragma once

#ifdef LICHT_SCHEMA_EXPORTS
#ifdef _MSC_VER
#define LICHT_SCHEMA_API __declspec(dllexport)
#else
#define LICHT_SCHEMA_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LICHT_SCHEMA_API __declspec(dllimport)
#else
#define LICHT_SCHEMA_API
#endif
#endif