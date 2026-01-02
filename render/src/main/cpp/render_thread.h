#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include <native_vsync/native_vsync.h>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
#include "egl_core.h"
#include "renderer_interface.h"

class RenderThread {
public:
    RenderThread();
    ~RenderThread();

    void Start(void* window, int width, int height);
    void UpdateSize(int width, int height);
    void TogglePause();
    void Stop();

    void SetRenderer(IRenderer* renderer);

private:
    void RenderLoop();
    static void OnVSync(long long timestamp, void* data);

    void* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    EGLCore* eglCore_ = nullptr;
    IRenderer* renderer_ = nullptr; // Owned by caller or thread? Let's say owned by thread for now, or just referenced.
    // For this refactor, let's assume PluginRender creates and owns RenderThread, and sets a Renderer.
    // Ideally RenderThread should own Renderer or manage its lifecycle.

    std::thread thread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::mutex mtx_;
    std::condition_variable cv_;
    bool frameReady_ = false;
    
    OH_NativeVSync* nativeVSync_ = nullptr;
};

#endif // RENDER_THREAD_H
