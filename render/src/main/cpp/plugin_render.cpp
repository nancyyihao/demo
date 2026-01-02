#include "plugin_render.h"
#include "render_common.h"
#include <ace/xcomponent/native_interface_xcomponent.h>

PluginRender* PluginRender::instance_ = nullptr;

PluginRender* PluginRender::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new PluginRender();
    }
    return instance_;
}

// Static Callbacks mapping to instance methods
void PluginRender::OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
    if (instance_) instance_->OnSurfaceCreated(component, window);
}

void PluginRender::OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
    if (instance_) instance_->OnSurfaceChanged(component, window);
}

void PluginRender::OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
    if (instance_) instance_->OnSurfaceDestroyed(component, window);
}

void PluginRender::DispatchTouchEventCB(OH_NativeXComponent* component, void* window) {
    if (instance_) instance_->DispatchTouchEvent(component, window);
}

void PluginRender::Export(napi_env env, napi_value exports) {
    if (env == nullptr || exports == nullptr) {
        return;
    }
    napi_value exportInstance = nullptr;
    if (napi_get_reference_value(env, nullptr, &exportInstance) != napi_ok) {
        // Handle error handling if needed, but for simple export:
    }
    // In this simple case, we are just exporting global functions or properties if needed.
    // The main NAPI export is handled in napi_init.cpp
}

napi_value PluginRender::TogglePause(napi_env env, napi_callback_info info) {
    LOGI("TogglePause called");
    if (instance_ && instance_->renderThread_) {
        instance_->renderThread_->TogglePause();
    }
    return nullptr;
}

void PluginRender::SetNativeXComponent(std::string& id, OH_NativeXComponent* component) {
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceChanged = OnSurfaceChangedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    renderCallback_.DispatchTouchEvent = DispatchTouchEventCB;
    OH_NativeXComponent_RegisterCallback(component, &renderCallback_);
}

void PluginRender::OnSurfaceCreated(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceCreated");
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    
    if (renderThread_ == nullptr) {
        renderThread_ = new RenderThread();
        renderThread_->Start(window, width, height);
    }
}

void PluginRender::OnSurfaceChanged(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceChanged");
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if (renderThread_) {
        renderThread_->UpdateSize(width, height);
    }
}

void PluginRender::OnSurfaceDestroyed(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceDestroyed");
    if (renderThread_) {
        renderThread_->Stop();
        delete renderThread_;
        renderThread_ = nullptr;
    }
}

void PluginRender::DispatchTouchEvent(OH_NativeXComponent* component, void* window) {
    OH_NativeXComponent_TouchEvent touchEvent;
    if (OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        if (touchEvent.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UP) {
            LOGI("Touch UP detected, toggling pause");
            if (renderThread_) {
                renderThread_->TogglePause();
            }
        }
    }
}
