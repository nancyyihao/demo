#ifndef PLUGIN_RENDER_H
#define PLUGIN_RENDER_H

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>
#include <string>
#include <unordered_map>
#include "render_thread.h"

class PluginRender {
public:
    static PluginRender* GetInstance();
    static napi_value TogglePause(napi_env env, napi_callback_info info);

    void Export(napi_env env, napi_value exports);
    void SetNativeXComponent(std::string& id, OH_NativeXComponent* component);
    void ReleaseNativeXComponent(std::string& id);

public:
    // Callback methods for XComponent
    static void OnSurfaceCreatedCB(OH_NativeXComponent* component, void* window);
    static void OnSurfaceChangedCB(OH_NativeXComponent* component, void* window);
    static void OnSurfaceDestroyedCB(OH_NativeXComponent* component, void* window);
    static void DispatchTouchEventCB(OH_NativeXComponent* component, void* window);

private:
    void OnSurfaceCreated(OH_NativeXComponent* component, void* window);
    void OnSurfaceChanged(OH_NativeXComponent* component, void* window);
    void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window);
    void DispatchTouchEvent(OH_NativeXComponent* component, void* window);

    static PluginRender* instance_;
    RenderThread* renderThread_ = nullptr;
    OH_NativeXComponent_Callback renderCallback_;
};

#endif // PLUGIN_RENDER_H
