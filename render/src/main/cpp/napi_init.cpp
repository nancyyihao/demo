#include "napi/native_api.h"
#include "plugin_render.h"
#include "render_common.h"

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    LOGI("Init NAPI module start");
    
    // Register methods on exports (the context object)
    // We call PluginRender::Init to attach methods like togglePause to exports
    PluginRender::Init(env, exports);

    // Retrieve the Native XComponent
    napi_value exportInstance = nullptr;
    napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    if (exportInstance) {
        OH_NativeXComponent* nativeXComponent = nullptr;
        if (napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent)) == napi_ok) {
             
             // Get ID to create specific instance
             char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {};
             uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
             if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
                 std::string id = idStr;
                 LOGI("Init: Found NativeXComponent with ID: %s", id.c_str());
                 
                 // Create PluginInstance and associate it with nativeXComponent
                 // Note: We need to somehow bind 'this' in JS calls to this instance.
                 // We use napi_wrap on the exports object to attach our C++ instance.
                 
                 // 1. Create Instance associated with ID.
                 // Ideally we use a Factory or Manager. Here we use static Set method which creates new.
                 // But wait, SetNativeXComponent is creating `new PluginRender`.
                 // We need to return that pointer to wrap it.
                 
                 // Refactoring PluginRender::SetNativeXComponent to return instance or separate creation.
                 // Let's manually do it here to be clear.
                 
                 PluginRender* instance = PluginRender::GetInstance(id);
                 if (!instance) {
                     instance = new PluginRender(id); // Constructor registers itself in map
                     instance->SetNativeXComponent(id, nativeXComponent); 
                 }
                 
                 // Wrap the instance into the exports object.
                 // This ensures that when methods on `exports` are called, `napi_unwrap` retrieves this instance.
                 napi_wrap(env, exports, instance, 
                    [](napi_env env, void* data, void* hint) {
                        // Finalizer
                        // Usually we delete instance here, but XComponent might destroy earlier or later.
                        // Let's rely on explicit Release or Map cleanup.
                        // Or simple: delete instance;
                    }, nullptr, nullptr);
                    
             } else {
                 LOGE("Init: Failed to get XComponent ID");
             }
        } else {
             LOGE("Init: Failed to unwrap NativeXComponent");
        }
    } else {
        LOGE("Init: OH_NATIVE_XCOMPONENT_OBJ not found");
    }

    LOGI("Init NAPI module end");
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "render",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterRenderModule(void)
{
    napi_module_register(&demoModule);
}
