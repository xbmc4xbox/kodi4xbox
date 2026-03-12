#include <windows.h>
#include <stdio.h>
#include <limits.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "errno.h"

int64_t nxio_lseek_lk(nxcrt_file_t *file, int64_t offset, int whence) {
    if (file->type != NXCRT_FD_TYPE_WIN32) {
        errno = ESPIPE;
        return (off_t)-1;
    }

    LARGE_INTEGER li_offset;
    LARGE_INTEGER li_newpos;

    li_offset.QuadPart = offset;

    DWORD move_method;

    switch (whence) {
        case SEEK_SET:
            move_method = FILE_BEGIN;
            break;
        case SEEK_CUR:
            move_method = FILE_CURRENT;
            break;
        case SEEK_END:
            move_method = FILE_END;
            break;
        default:
            errno = EINVAL;
            return (off_t)-1;
    }

    HANDLE osHandle = (HANDLE)file->handle;

    if (osHandle == INVALID_HANDLE_VALUE)
    {
        errno = EBADF;
        return -1;
    }

    BOOL ok = SetFilePointerEx(
        osHandle,
        li_offset,
        &li_newpos,
        move_method
    );

    if (!ok) {
        nxcrt_errno_win32(GetLastError());

        return (off_t)-1;
    }

    file->offset = (int64_t)li_newpos.QuadPart;
    file->status_flags &= ~NXCRT_FEOFLAG;

    return file->offset;
}
