#include "render_thread.h"
#include "render_common.h"

RenderThread::RenderThread() {}

RenderThread::~RenderThread() {
    Stop();
}

void RenderThread::SetRenderer(IRenderer* renderer) {
    renderer_ = renderer;
}

void RenderThread::Start(void* window, int width, int height) {
    LOGI("RenderThread Start");
    window_ = window;
    width_ = width;
    height_ = height;
    running_ = true;
    thread_ = std::thread(&RenderThread::RenderLoop, this);
}

void RenderThread::UpdateSize(int width, int height) {
    LOGI("RenderThread UpdateSize: %d %d", width, height);
    std::lock_guard<std::mutex> lock(mtx_);
    width_ = width;
    height_ = height;
    // We can't call GL here directly as this is usually UI thread.
    // Flags or Atomic sizes could work, but RenderLoop will handle it if we re-sync context or just let Renderer handle it.
    // Actually EGLCore needs to know, but glViewport is context specific. 
    // We should probably post a message or simplify.
    // For now, let's assume the loop picks up the new size or we just set it and let next frame handle. Use atomic or protected var.
    // Ideally we'd have a message queue. Simplified: Just update `width_` `height_` and let Loop read it (protected by mutex inside loop if needed, but int is usually atomic enough for this simple demo).
    // Better: Helper method inside loop to check size change.
}

void RenderThread::TogglePause() {
    bool expected = paused_.load();
    paused_.store(!expected);
    LOGI("RenderThread TogglePause: %d", paused_.load());
}

void RenderThread::Stop() {
    LOGI("RenderThread Stop");
    running_ = false;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        cv_.notify_all(); // Wake up wait
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void RenderThread::OnVSync(long long timestamp, void* data) {
    RenderThread* thread = static_cast<RenderThread*>(data);
    if (thread) {
        std::lock_guard<std::mutex> lock(thread->mtx_);
        thread->frameReady_ = true;
        thread->cv_.notify_one();
    }
}

void RenderThread::RenderLoop() {
    eglCore_ = new EGLCore();
    if (!eglCore_->Init(window_, width_, height_)) {
        LOGE("Failed to init EGLCore");
        return;
    }

    if (renderer_) {
        renderer_->OnSurfaceCreated(eglCore_);
        renderer_->OnSurfaceChanged(width_, height_);
    }

    // Create VSync instance
    nativeVSync_ = OH_NativeVSync_Create("render_vsync", strlen("render_vsync"));
    if (!nativeVSync_) {
        LOGE("Failed to create NativeVSync");
    }

    while (running_) {
        // Handle size change if necessary? With EGL, usually surface might need update or just glViewport.
        // For simplicity: Check if size changed compared to last known.
        static int lastW = width_;
        static int lastH = height_;
        if (width_ != lastW || height_ != lastH) {
             lastW = width_;
             lastH = height_;
             glViewport(0, 0, lastW, lastH);
             if (renderer_) renderer_->OnSurfaceChanged(lastW, lastH);
        }

        if (nativeVSync_ && !paused_) {
            OH_NativeVSync_RequestFrame(nativeVSync_, &RenderThread::OnVSync, this);
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return frameReady_ || !running_; });
            if (!running_) break;
            frameReady_ = false;
        } else if (paused_) {
             // Avoid busy loop if paused
             std::this_thread::sleep_for(std::chrono::milliseconds(100));
             continue;
        }

        if (renderer_) {
            renderer_->Draw();
        }
        eglCore_->Swap();
    }

    if (nativeVSync_) {
        OH_NativeVSync_Destroy(nativeVSync_);
        nativeVSync_ = nullptr;
    }

    if (renderer_) {
        renderer_->Destroy();
    }
    eglCore_->Destroy();
    delete eglCore_;
    eglCore_ = nullptr;
}
