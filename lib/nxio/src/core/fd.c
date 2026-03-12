#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "nxio/fd.h"
#include "nxio/log.h"

#include "errno.h"
#include "fcntl.h"

nxcrt_fd_t nxcrt_fd_table[NXCRT_MAX_FDS];
CRITICAL_SECTION nxcrt_fd_table_lock;

static void nxcrt_fd_close_underlying(nxcrt_file_t *file);

void nxcrt_fd_runtime_init(void)
{
    nxio_print_debug("Initializing fd.c");
    InitializeCriticalSection(&nxcrt_fd_table_lock);
    memset(nxcrt_fd_table, 0, sizeof(nxcrt_fd_table));
    FILE *std_files[3] = { stdin, stdout, stderr };
    int std_flags[3]   = { NX_O_RDONLY, NX_O_WRONLY, NX_O_WRONLY };
    for (int i = 0; i < 3 && i < NXCRT_MAX_FDS; ++i)
    {
        nxcrt_file_t *file = nxcrt_file_create(
            NXCRT_FD_TYPE_STDIO,
            std_flags[i],
            std_files[i]
        );

        if (!file)
            continue;

        nxcrt_fd_table[i].used = 1;
        nxcrt_fd_table[i].file = file;
    }
    nxio_print_debug("Initialized fd.c");
}

__attribute__((used))
__attribute__((section(".CRT$XCU")))
static void (*nxcrt_fd_init_ptr)(void) = nxcrt_fd_runtime_init;

nxcrt_file_t *nxcrt_fd_get(int fd)
{
    if (fd < 0 || fd >= NXCRT_MAX_FDS)
    {
        errno = EBADF;
        return NULL;
    }

    EnterCriticalSection(&nxcrt_fd_table_lock);

    if (!nxcrt_fd_table[fd].used || !nxcrt_fd_table[fd].file)
    {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        errno = EBADF;
        return NULL;
    }

    nxcrt_file_t *file = nxcrt_fd_table[fd].file;

    EnterCriticalSection(&file->lock);
    file->refcount++;
    LeaveCriticalSection(&file->lock);

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    return file;
}

int nxcrt_fd_release(int fd)
{
    if (fd < 3 || fd >= NXCRT_MAX_FDS) {
        errno = EBADF;
        return -1;
    }

    EnterCriticalSection(&nxcrt_fd_table_lock);

    if (!nxcrt_fd_table[fd].used ||
        !nxcrt_fd_table[fd].file)
    {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        errno = EBADF;
        return -1;
    }

    nxcrt_file_t *file = nxcrt_fd_table[fd].file;

    /* Now protect refcount */
    EnterCriticalSection(&file->lock);
    file->refcount--;
    int should_close = (file->refcount == 0);
    LeaveCriticalSection(&file->lock);

    if (should_close) {
        nxcrt_fd_table[fd].used = 0;
        nxcrt_fd_table[fd].file = NULL;
    }

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    if (should_close)
    {
        nxcrt_fd_close_underlying(file);
        DeleteCriticalSection(&file->lock);
        free(file);
    }

    return 0;
}

void nxcrt_file_lock(nxcrt_file_t *file)
{
    if (!file) return;
    EnterCriticalSection(&file->lock);
}

void nxcrt_file_unlock(nxcrt_file_t *file)
{
    if (!file) return;
    LeaveCriticalSection(&file->lock);
}

void nxcrt_fd_close_underlying(nxcrt_file_t *file)
{
    if (!file || !file->handle)
        return;

    switch (file->type)
    {
        case NXCRT_FD_TYPE_FILE:
        case NXCRT_FD_TYPE_STDIO:
        {
            FILE *f = (FILE *)file->handle;

            if (f != stdin && f != stdout && f != stderr)
            {
                fclose(f);
            }
            break;
        }

        case NXCRT_FD_TYPE_WIN32:
        {
            HANDLE h = (HANDLE)file->handle;

            if (h != NULL && h != INVALID_HANDLE_VALUE)
            {
                if (CloseHandle(h) == 0) {
                    nxio_print_debug("Couldn't close underlying HANDLE. Win32 error: %d", GetLastError());
                }
            }
            break;
        }

        case NXCRT_FD_TYPE_NONE:
        default:
            break;
    }

    file->handle = NULL;
}

// Installs file. Returns assigned fd. Doesn't increment refcount.
int nxcrt_fd_install(nxcrt_file_t *file)
{
    int fd = -1;

    EnterCriticalSection(&nxcrt_fd_table_lock);
    for (int i = 3; i < NXCRT_MAX_FDS; ++i)
    {
        if (!nxcrt_fd_table[i].used)
        {
            fd = i;
            nxcrt_fd_table[i].used = 1;
            nxcrt_fd_table[i].file = file;
            break;
        }
    }

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    if (fd < 0)
        errno = EMFILE;

    return fd;
}

nxcrt_file_t *nxcrt_file_create(nxcrt_fd_type_t type,
                                       int flags,
                                       void *handle)
{
    nxcrt_file_t *file = malloc(sizeof(nxcrt_file_t));

    if (!file)
        return NULL; // OOM

    file->type = type;
    file->flags = flags;
    file->offset = 0;
    file->status_flags = 0;
    file->refcount = 1;
    file->handle = handle;

    InitializeCriticalSection(&file->lock);

    return file;
}