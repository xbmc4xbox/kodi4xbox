#include <windows.h>

#include "nxio/log.h"
#include "nxio/errno_win32.h"
#include "errno.h"


void nxcrt_errno_win32(unsigned long err) {
    nxio_print_debug("win32_error: %d", err);

    switch (err)
    {
        case ERROR_INVALID_FUNCTION:
        case ERROR_INVALID_ACCESS:
        case ERROR_INVALID_DATA:
        case ERROR_INVALID_PARAMETER:
        case ERROR_INVALID_HANDLE:
        case ERROR_NEGATIVE_SEEK:
            errno = EINVAL;
            break;
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
        case ERROR_INVALID_DRIVE:
        case ERROR_NO_MORE_FILES:
        case ERROR_BAD_NETPATH:
        case ERROR_BAD_NET_NAME:
        case ERROR_BAD_PATHNAME:
        case ERROR_FILENAME_EXCED_RANGE:
            errno = ENOENT;
            break;
        case ERROR_TOO_MANY_OPEN_FILES:
            errno = EMFILE;
            break;
        case ERROR_ACCESS_DENIED:
        case ERROR_CURRENT_DIRECTORY:
        case ERROR_LOCK_VIOLATION:
        case ERROR_NETWORK_ACCESS_DENIED:
        case ERROR_CANNOT_MAKE:
        case ERROR_FAIL_I24:
        case ERROR_DRIVE_LOCKED:
        case ERROR_SEEK_ON_DEVICE:
        case ERROR_NOT_LOCKED:
        case ERROR_LOCK_FAILED:
            errno = EACCES;
            break;
        case ERROR_INVALID_TARGET_HANDLE:
        case ERROR_DIRECT_ACCESS_HANDLE:
            errno = EBADF;
            break;
        case ERROR_ARENA_TRASHED:
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_INVALID_BLOCK:
        case ERROR_NOT_ENOUGH_QUOTA:
            errno = ENOMEM;
            break;
        case ERROR_BAD_ENVIRONMENT:
            errno = E2BIG;
            break;
        case ERROR_BAD_FORMAT:
            errno = ENOEXEC;
            break;
        case ERROR_NOT_SAME_DEVICE:
            errno = EXDEV;
            break;
        case ERROR_FILE_EXISTS:
            errno = EEXIST;
            break;
        case ERROR_NO_PROC_SLOTS:
        case ERROR_MAX_THRDS_REACHED:
        case ERROR_NESTING_NOT_ALLOWED:
            errno = EAGAIN;
            break;
        case ERROR_BROKEN_PIPE:
            errno = EPIPE;
            break;
        case ERROR_DISK_FULL:
            errno = ENOSPC;
            break;
        case ERROR_WAIT_NO_CHILDREN:
        case ERROR_CHILD_NOT_COMPLETE:
            errno = ECHILD;
            break;
        case ERROR_DIR_NOT_EMPTY:
            errno = ENOTEMPTY;
            break;
        default:
            errno = EIO;
            break;
    }
}
