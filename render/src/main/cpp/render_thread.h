#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include <native_vsync/native_vsync.h>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include "egl_core.h"
#include "renderer_interface.h"

class RenderThread {
public:
    using Task = std::function<void()>;

    RenderThread();
    ~RenderThread();

    void Start(void* window, int width, int height);
    void UpdateSize(int width, int height);
    void TogglePause();
    void Stop();

    void SetRenderer(IRenderer* renderer);
    void PostTask(Task task);

private:
    void RenderLoop();
    static void OnVSync(long long timestamp, void* data);

    void* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    EGLCore* eglCore_ = nullptr;
    IRenderer* renderer_ = nullptr;

    std::thread thread_;
    std::atomic<bool> running_{false};
    bool paused_ = false;
    
    std::mutex mtx_;
    std::condition_variable cv_;
    std::deque<Task> taskQueue_;
    
    OH_NativeVSync* nativeVSync_ = nullptr;
};

#endif // RENDER_THREAD_H
