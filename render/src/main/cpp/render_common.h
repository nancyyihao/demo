#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#include <hilog/log.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RenderModule"

#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#endif
#define LOG_DOMAIN 0x0001

#define LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#endif // RENDER_COMMON_H
