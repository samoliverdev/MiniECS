#pragma once

#include <ECS.h>

#if defined(_WIN32)
    #if defined(SAMPLE_ENGINE_BUILD_DLL)
        #define SAMPLE_ENGINE_API __declspec(dllexport)
    #else
        #define SAMPLE_ENGINE_API __declspec(dllimport)
    #endif
#else
    #define SAMPLE_ENGINE_API
#endif

struct SAMPLE_ENGINE_API DllPosition {
    CompDefinition
    float x = 0.0f;
    float y = 0.0f;
};

struct SAMPLE_ENGINE_API DllVelocity {
    CompDefinition
    float x = 0.0f;
    float y = 0.0f;
};

SAMPLE_ENGINE_API void RegisterDllSampleComponents();
