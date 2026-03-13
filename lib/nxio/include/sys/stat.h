#pragma once

#include <stdint.h>
#include <time.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define S_IFMT   0xF000
#define S_IFDIR  0x4000
#define S_IFCHR  0x2000
#define S_IFIFO  0x1000
#define S_IFREG  0x8000

#define S_IREAD   0x0100
#define S_IWRITE  0x0080
#define S_IEXEC   0x0040

#ifdef NXIO_POSIX

    struct stat {
        dev_t     st_dev;
        ino_t     st_ino;
        mode_t    st_mode;
        nlink_t   st_nlink;
        uid_t     st_uid;
        gid_t     st_gid;
        dev_t     st_rdev;
        off_t     st_size;
        time_t    st_atime;
        time_t    st_mtime;
        time_t    st_ctime;
    };

    int fstat(int fd, struct stat *buf);
    int stat(const char *path, struct stat *buf);

    #define S_IFMT   _S_IFMT
    #define S_IFDIR  _S_IFDIR
    #define S_IFCHR  _S_IFCHR
    #define S_IFIFO  _S_IFIFO
    #define S_IFREG  _S_IFREG

    #define S_IRUSR  _S_IREAD
    #define S_IWUSR  _S_IWRITE
    #define S_IXUSR  _S_IEXEC

#else // NXIO_POSIX

    // 32-bit size/time
    struct _stat {
        int32_t  st_dev;
        int32_t  st_ino;
        int16_t  st_mode;
        int16_t  st_nlink;
        int32_t  st_uid;
        int32_t  st_gid;
        int32_t  st_rdev;
        int32_t  st_size;
        int32_t  st_atime;
        int32_t  st_mtime;
        int32_t  st_ctime;
    };

    // 64-bit size, 32-bit time (MSVC classic behavior)
    struct _stati64 {
        int32_t  st_dev;
        int64_t  st_ino;
        int16_t  st_mode;
        int16_t  st_nlink;
        int32_t  st_uid;
        int32_t  st_gid;
        int32_t  st_rdev;
        int64_t  st_size;
        int32_t  st_atime;
        int32_t  st_mtime;
        int32_t  st_ctime;
    };

    // 64-bit size, 64-bit time (MSVC modern behavior)
    struct _stat64 {
        int32_t  st_dev;
        int64_t  st_ino;
        int16_t  st_mode;
        int16_t  st_nlink;
        int32_t  st_uid;
        int32_t  st_gid;
        int32_t  st_rdev;
        int64_t  st_size;
        int64_t  st_atime;
        int64_t  st_mtime;
        int64_t  st_ctime;
    };

    #define __stat64 _stat64

    int _fstat32(int fd, struct _stat *buf);
    int _fstati64(int fd, struct _stati64 *buf);
    int _fstat64(int fd, struct _stat64 *buf);

    int _stat(const char *path, struct _stat *buf);
    int _stati64(const char *path, struct _stati64 *buf);
    int _stat64(const char *path, struct _stat64 *buf);

#endif

#ifdef __cplusplus
}
#endif
