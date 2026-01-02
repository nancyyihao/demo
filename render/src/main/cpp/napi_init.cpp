#include "napi/native_api.h"
#include "plugin_render.h"
#include "render_common.h"

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    LOGI("Init NAPI module start");
    napi_property_descriptor desc[] = {
        {"togglePause", nullptr, PluginRender::TogglePause, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    PluginRender::GetInstance()->Export(env, exports);

    // Retrieve the Native XComponent
    napi_value exportInstance = nullptr;
    napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    if (exportInstance) {
        OH_NativeXComponent* nativeXComponent = nullptr;
        if (napi_unwrap(env, exportInstance, reinterpret_cast<void**>(&nativeXComponent)) == napi_ok) {
             LOGI("Init: Found NativeXComponent");
             std::string id = "render_id"; // Default or retrieve
             PluginRender::GetInstance()->SetNativeXComponent(id, nativeXComponent);
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
