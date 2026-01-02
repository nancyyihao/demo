#ifndef RENDERER_INTERFACE_H
#define RENDERER_INTERFACE_H

#include "egl_core.h"

class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    // Called when EGL context is created and made current.
    // Initialize Shaders, VBOs, etc here.
    virtual void OnSurfaceCreated(EGLCore* eglCore) = 0;
    
    // Called when window size changes
    virtual void OnSurfaceChanged(int width, int height) = 0;
    
    // Called every frame to draw
    virtual void Draw() = 0;
    
    // Cleanup resources
    virtual void Destroy() = 0;
};

#endif // RENDERER_INTERFACE_H
