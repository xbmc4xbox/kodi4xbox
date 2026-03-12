#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NXIO_POSIX

    long nxio_lseek32(int fd, long offset, int whence);
    int64_t nxio_lseeki64(int fd, int64_t offset, int whence);

    int nxio_read32(int fd, void *buf, unsigned int cnt);
    int nxio_write32(int fd, const void *buf, unsigned int cnt);

    int nxio_open(const char *path, int oflag, ...);
    int nxio_sopen(const char *path, int oflag, int shflag, ...);
    int nxio_close(int fd);
    int nxio_dup(int fd);
    int nxio_dup2(int oldfd, int newfd);
    int nxio_unlink(const char *path);
    int nxio_access(const char *path, int amode);

    #define _open      nxio_open
    #define _sopen     nxio_sopen
    #define _close     nxio_close
    #define _dup       nxio_dup
    #define _dup2      nxio_dup2
    #define _unlink    nxio_unlink
    #define _access    nxio_access

    #define _lseek     nxio_lseek32
    #define _lseeki64  nxio_lseeki64

    #define _read      nxio_read32
    #define _write     nxio_write32

#else /* !NXIO_POSIX */

    #warning "<io.h> has been included in POSIX mode. This is usually a mistake."

#endif /* !NXIO_POSIX */

#ifdef __cplusplus
}
#endif
