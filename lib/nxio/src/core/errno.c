#include <windows.h>
#include <stdlib.h>
#include "errno.h"

static DWORD nxcrt_errno_fls_index = FLS_OUT_OF_INDEXES;
static INIT_ONCE nxcrt_errno_init_once = INIT_ONCE_STATIC_INIT;

static VOID CALLBACK nxcrt_errno_destructor(PVOID value)
{
    if (value) {
        free(value);
    }
}

static BOOL CALLBACK nxcrt_errno_init_once_fn(
    PINIT_ONCE InitOnce,
    PVOID Parameter,
    PVOID *Context)
{
    (void)InitOnce;
    (void)Parameter;
    (void)Context;

    nxcrt_errno_fls_index = FlsAlloc(nxcrt_errno_destructor);

    return (nxcrt_errno_fls_index != FLS_OUT_OF_INDEXES);
}

int *__nxcrt_errno_location(void)
{
    if (!InitOnceExecuteOnce(
            &nxcrt_errno_init_once,
            nxcrt_errno_init_once_fn,
            NULL,
            NULL))
    {
        return NULL; // FIXME: Initialization not possible. Why?
    }

    int *per_thread_errno =
        (int *)FlsGetValue(nxcrt_errno_fls_index);

    if (!per_thread_errno) {
        per_thread_errno = (int *)calloc(1, sizeof(int));
        if (!per_thread_errno) {
            return NULL; // FIXME: OOM?. Should log this.
        }

        FlsSetValue(nxcrt_errno_fls_index, per_thread_errno);
    }

    return per_thread_errno;
}
