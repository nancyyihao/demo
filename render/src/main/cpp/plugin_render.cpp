#include "plugin_render.h"
#include "render_common.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <string>
#include "samples/sine_wave_renderer.h"

std::unordered_map<std::string, PluginRender*> PluginRender::instanceMap_;

PluginRender* PluginRender::GetInstance(std::string& id) {
    if (instanceMap_.find(id) == instanceMap_.end()) {
        return nullptr;
    }
    return instanceMap_[id];
}

void PluginRender::ReleaseInstance(std::string& id) {
    if (instanceMap_.find(id) != instanceMap_.end()) {
        PluginRender* instance = instanceMap_[id];
        delete instance;
        instanceMap_.erase(id);
    }
}

PluginRender::PluginRender(std::string& id) : id_(id) {
    instanceMap_[id] = this;
}

PluginRender::~PluginRender() {
    if (renderThread_) {
        delete renderThread_;
    }
    if (renderer_) {
        delete renderer_;
    }
}

napi_value PluginRender::Init(napi_env env, napi_value exports) {
    // We don't create instance here anymore. We wait for XComponent ID injection.
    // However, the current template injects ID via separate method or we just handle wrapping.
    // Actually, usually Init is called once per module load.
    // We need to export a Constructor or Factory method.
    // BUT for simplicity in this demo, let's keep the `togglePause` method on the prototype 
    // or instance.
    // The previous code had `PluginRender::GetInstance()->Export`.
    
    // Updated Logic:
    // 1. Export `togglePause` essentially as a method that operates on `this` context?
    // ArkTS code: `testNapi.togglePause()` was called on the module.
    // To support multi-instance: `context.togglePause()` where context is the instance.
    
    napi_property_descriptor desc[] = {
        {"togglePause", nullptr, PluginRender::TogglePause, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    
    return exports;
}

// Static Callbacks mapping to instance methods
// Challenge: callback only gives component + window.
// We need to map component -> instance.
// OH_NativeXComponent_GetXComponentId is key.

void PluginRender::OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window) {
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        std::string id(idStr);
        PluginRender* instance = GetInstance(id);
        if (instance) instance->OnSurfaceCreated(component, window);
    }
}

void PluginRender::OnSurfaceChangedCB(OH_NativeXComponent* component, void* window) {
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
         std::string id(idStr);
         PluginRender* instance = GetInstance(id);
         if (instance) instance->OnSurfaceChanged(component, window);
    }
}

void PluginRender::OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window) {
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
         std::string id(idStr);
         PluginRender* instance = GetInstance(id);
         if (instance) instance->OnSurfaceDestroyed(component, window);
    }
}

void PluginRender::DispatchTouchEventCB(OH_NativeXComponent* component, void* window) {
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
         std::string id(idStr);
         PluginRender* instance = GetInstance(id);
         if (instance) instance->DispatchTouchEvent(component, window);
    }
}

napi_value PluginRender::TogglePause(napi_env env, napi_callback_info info) {
    LOGI("TogglePause called");
    // How do we get the instance?
    // We can wrap the instance into the 'this' object of the callback info.
    // napi_unwrap(env, thisVar, (void**)&instance);
    
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    
    PluginRender* instance = nullptr;
    napi_unwrap(env, thisVar, (void**)&instance);
    
    if (instance && instance->renderThread_) {
        instance->renderThread_->TogglePause();
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


#include "samples/cosine_wave_renderer.h"

void PluginRender::OnSurfaceCreated(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceCreated %s", id_.c_str());
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    
    if (renderThread_ == nullptr) {
        renderThread_ = new RenderThread();
        // Choose renderer based on ID
        if (id_.find("cos") != std::string::npos) {
             LOGI("Using CosineWaveRenderer");
             renderer_ = new CosineWaveRenderer();
        } else {
             LOGI("Using SineWaveRenderer");
             renderer_ = new SineWaveRenderer();
        }
        
        renderThread_->SetRenderer(renderer_);
        renderThread_->Start(window, width, height);
    }
}

void PluginRender::OnSurfaceChanged(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceChanged %s", id_.c_str());
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if (renderThread_) {
        renderThread_->UpdateSize(width, height);
    }
}

void PluginRender::OnSurfaceDestroyed(OH_NativeXComponent* component, void* window) {
    LOGI("OnSurfaceDestroyed %s", id_.c_str());
    if (renderThread_) {
        renderThread_->Stop();
        delete renderThread_;
        renderThread_ = nullptr;
    }
    // Don't delete self here yet, wait for Release or implicit app lifecycle?
    // MapLibre usually keeps it if view might be recreated.
    // But map cleanup is good practice.
    // ReleaseInstance(id_); // Be careful if JS implementation still holds reference.
}

void PluginRender::DispatchTouchEvent(OH_NativeXComponent* component, void* window) {
    OH_NativeXComponent_TouchEvent touchEvent;
    if (OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        if (touchEvent.type == OH_NativeXComponent_TouchEventType::OH_NATIVEXCOMPONENT_UP) {
            LOGI("Touch UP detected, toggling pause for %s", id_.c_str());
            if (renderThread_) {
                renderThread_->TogglePause();
            }
        }
    }
}
