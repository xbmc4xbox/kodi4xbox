#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void nxio_print_debug_impl(const char *fmt, ...);

// #define NXIO_DEBUG
#ifdef NXIO_DEBUG
#define nxio_print_debug(...) nxio_print_debug_impl(__VA_ARGS__)
#else
#define nxio_print_debug(...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif
