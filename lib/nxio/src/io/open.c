#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/stat.h"

int nxio_sopen_impl(const char *path,
                           int oflag,
                           int shflag,
                           int pmode)
{
    DWORD access = 0;
    DWORD share = 0;
    DWORD creation = 0;
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;

    if ((oflag & NX_O_EXCL) && !(oflag & NX_O_CREAT)) {
        errno = EINVAL;
        return -1;
    }

    switch (oflag & NX_O_ACCMODE) {
        case NX_O_RDONLY:
            access = GENERIC_READ;
            break;

        case NX_O_WRONLY:
            access = GENERIC_WRITE;
            break;

        case NX_O_RDWR:
            access = GENERIC_READ | GENERIC_WRITE;
            break;

        default:
            errno = EINVAL;
            return -1;
    }

    if (oflag & NX_O_TRUNC)
        access |= GENERIC_WRITE;

    switch (shflag) {
        case NX_SH_DENYRW:
            share = 0;
            break;

        case NX_SH_DENYWR:
            share = FILE_SHARE_READ;
            break;

        case NX_SH_DENYRD:
            share = FILE_SHARE_WRITE;
            break;

        case NX_SH_DENYNO:
            share = FILE_SHARE_READ | FILE_SHARE_WRITE;
            break;

        default:
            errno = EINVAL;
            return -1;
    }

    switch (oflag & (NX_O_CREAT | NX_O_EXCL | NX_O_TRUNC)) {

        case 0:
            creation = OPEN_EXISTING;
            break;

        case NX_O_CREAT:
            creation = OPEN_ALWAYS;
            break;

        case NX_O_CREAT | NX_O_EXCL:
            creation = CREATE_NEW;
            break;

        case NX_O_TRUNC:
            creation = TRUNCATE_EXISTING;
            break;

        case NX_O_CREAT | NX_O_TRUNC:
            creation = CREATE_ALWAYS;
            break;

        case NX_O_CREAT | NX_O_TRUNC | NX_O_EXCL:
            creation = CREATE_NEW;
            break;

        default:
            errno = EINVAL;
            return -1;
    }

    if (oflag & NX_O_CREAT) {
        if (!(pmode & _S_IWRITE))
            attributes |= FILE_ATTRIBUTE_READONLY;
    }

    if (oflag & NX_O_TEMPORARY) {
        attributes |= FILE_FLAG_DELETE_ON_CLOSE;
        access |= DELETE;
        share |= FILE_SHARE_DELETE;
    }

    if (oflag & NX_O_SEQUENTIAL)
        attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
    else if (oflag & NX_O_RANDOM)
        attributes |= FILE_FLAG_RANDOM_ACCESS;

    HANDLE h = CreateFileA(
        path,
        access,
        share,
        NULL,
        creation,
        attributes,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE) {
        nxcrt_errno_win32(GetLastError());
        return -1;
    }

    nxcrt_file_t *file = nxcrt_file_create(
        NXCRT_FD_TYPE_WIN32,
        oflag,
        h
    );

    if (!file) {
        CloseHandle(h);
        errno = EMFILE;
        return -1;
    }

    if (oflag & NX_O_APPEND)
    {
        off_t pos = nxio_lseek_lk(file, 0, SEEK_END);
        if (pos == (off_t)-1) {
            nxcrt_fd_close_underlying(file);
            DeleteCriticalSection(&file->lock);
            free(file); 
            return -1;
        }
        file->offset = pos;
    }

    int fd = nxcrt_fd_install(file);
    if (fd < 0) {
        errno = EMFILE;
        nxcrt_fd_close_underlying(file);
        DeleteCriticalSection(&file->lock);
        free(file); 
        return -1;
    }

    return fd;
}

int nxio_open(const char *path, int oflag, ...)
{
    va_list ap;
    int pmode = 0;

    if (oflag & NX_O_CREAT) {
        va_start(ap, oflag);
        pmode = va_arg(ap, int);
        va_end(ap);
    }

    return nxio_sopen(path, oflag, NX_SH_DENYNO, pmode);
}

int nxio_sopen(const char *path, int oflag, int shflag, ...)
{
    int pmode = 0;

    if (oflag & NX_O_CREAT) {
        va_list ap;
        va_start(ap, shflag);
        pmode = va_arg(ap, int);
        va_end(ap);
    }

    return nxio_sopen_impl(path, oflag, shflag, pmode);
}
