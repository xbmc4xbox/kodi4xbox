#pragma once

#include <windows.h>

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NXCRT_MAX_FDS
#define NXCRT_MAX_FDS 64
#endif

#define NXCRT_FEOFLAG    0x0002

typedef enum {
    NXCRT_FD_TYPE_NONE = 0,
    NXCRT_FD_TYPE_FILE,
    NXCRT_FD_TYPE_WIN32,
    NXCRT_FD_TYPE_STDIO
} nxcrt_fd_type_t;

typedef struct nxcrt_file {
    nxcrt_fd_type_t type;
    int flags;
    int status_flags;
    int64_t offset;
    int refcount;
    CRITICAL_SECTION lock;
    void *handle;
} nxcrt_file_t;

typedef struct nxcrt_fd {
    int used;
    int fd_flags;
    nxcrt_file_t *file;
} nxcrt_fd_t;

// FD File API
void nxcrt_file_lock(nxcrt_file_t *file);
void nxcrt_file_unlock(nxcrt_file_t *file);
nxcrt_file_t *nxcrt_file_create(nxcrt_fd_type_t type, int flags, void *handle);

// FD Table API
nxcrt_file_t *nxcrt_fd_get(int fd);
int nxcrt_fd_release(int fd);
int nxcrt_fd_install(nxcrt_file_t *file);

// Internal API
void nxcrt_fd_close_underlying(nxcrt_file_t *file);

void nxcrt_fd_runtime_init(void);

extern CRITICAL_SECTION nxcrt_fd_table_lock;
extern nxcrt_fd_t nxcrt_fd_table[NXCRT_MAX_FDS];

#ifdef __cplusplus
}
#endif
