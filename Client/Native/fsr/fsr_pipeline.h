#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
    #define FSR_API __declspec(dllexport)
#else
    #define FSR_API __attribute__((visibility("default")))
#endif

// Check if current graphics environment supports FSR shaders
FSR_API bool fsr_is_supported();

// Initialize FSR pipeline resources (programs, VBOs, intermediate textures)
FSR_API bool fsr_init(void* sdlRenderer, int srcW, int srcH);

// Execute FSR passes (EASU + RCAS) and render to current display viewport
FSR_API bool fsr_render(
    void* sdlRenderer,
    void* sdlTexture,
    int srcW, int srcH,
    int dstX, int dstY, int dstW, int dstH,
    float sharpness
);

// Release all allocated GL and SDL resources
FSR_API void fsr_destroy();

#ifdef __cplusplus
}
#endif
