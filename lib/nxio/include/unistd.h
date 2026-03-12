#pragma once

#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NX_F_OK 0
#define NX_X_OK 1
#define NX_W_OK 2
#define NX_R_OK 4

#ifdef NXIO_POSIX

    #define F_OK NX_F_OK
    #define X_OK NX_X_OK
    #define W_OK NX_W_OK
    #define R_OK NX_R_OK

    int     nxio_close(int fd);
    int     nxio_dup(int fd);
    int     nxio_dup2(int oldfd, int newfd);
    int     nxio_unlink(const char *path);
    int     nxio_access(const char *path, int amode);

    off_t   nxio_lseek(int fd, off_t offset, int whence);
    ssize_t nxio_read_posix(int fd, void *buf, size_t count);
    ssize_t nxio_write_posix(int fd, const void *buf, size_t count);

    #define close   nxio_close
    #define dup     nxio_dup
    #define dup2    nxio_dup2
    #define unlink  nxio_unlink
    #define access  nxio_access

    #define lseek   nxio_lseek
    #define read    nxio_read_posix
    #define write   nxio_write_posix

#else /* NXIO_POSIX */

    #warning "<unistd.h> has been included in non POSIX mode. This is usually a mistake."

#endif /* NXIO_POSIX */

#ifdef __cplusplus
}
#endif

