#ifndef PLUGIN_RENDER_H
#define PLUGIN_RENDER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include <string>
#include <unordered_map>
#include "render_thread.h"
#include "renderer_interface.h"

class PluginRender {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value TogglePause(napi_env env, napi_callback_info info);

    static PluginRender* GetInstance(std::string& id);
    static void ReleaseInstance(std::string& id);

    void Export(napi_env env, napi_value exports);
    void SetNativeXComponent(std::string& id, OH_NativeXComponent* component);
    
public:
    // Callback methods for XComponent
    static void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window);
    static void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window);
    static void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window);
    static void DispatchTouchEventCB(OH_NativeXComponent* component, void* window);

public:
    PluginRender(std::string& id);
    ~PluginRender();

    void OnSurfaceCreated(OH_NativeXComponent* component, void* window);
    void OnSurfaceChanged(OH_NativeXComponent* component, void* window);
    void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window);
    void DispatchTouchEvent(OH_NativeXComponent* component, void* window);

    std::string id_;
    RenderThread* renderThread_ = nullptr;
    IRenderer* renderer_ = nullptr;
    OH_NativeXComponent_Callback renderCallback_;

    static std::unordered_map<std::string, PluginRender*> instanceMap_;
};

#endif // PLUGIN_RENDER_H
