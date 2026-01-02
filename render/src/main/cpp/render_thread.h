#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <atomic>
#include <thread>
#include <mutex>

class RenderThread {
public:
    RenderThread();
    ~RenderThread();

    void Start(void* window, int width, int height);
    void UpdateSize(int width, int height);
    void TogglePause();
    void Stop();

private:
    void RenderLoop();
    bool InitEGL();
    void DestroyEGL();
    void Draw();

    void* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLConfig eglConfig_;

    std::thread thread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::mutex mtx_;
    
    // Shader program
    GLuint program_;
    float offset_ = 0.0f;
};

#endif // RENDER_THREAD_H
