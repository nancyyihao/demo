#ifndef EGL_CORE_H
#define EGL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "render_common.h"

class EGLCore {
public:
    EGLCore();
    ~EGLCore();

    bool Init(void* window, int width, int height);
    void Draw(); // Pre-draw (clear, etc if needed, though usually Renderer does this)
    void Swap();
    void Destroy();
    void MakeCurrent();

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    
    // Helper to check errors
    bool CheckError(const char* msg);

private:
    void* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLConfig eglConfig_;
};

#endif // EGL_CORE_H
