#pragma once

#include <limits.h>
#include <stdint.h>

#ifdef NXIO_POSIX

    #if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
        typedef int64_t off_t;
        typedef int64_t ino_t;
        typedef int64_t dev_t;
        typedef int64_t blkcnt_t;
        typedef int32_t blksize_t;

        #define OFF_T_MAX INT64_MAX
        #define OFF_T_MIN INT64_MIN
    #else
        typedef int32_t off_t;
        typedef int32_t ino_t;
        typedef int32_t dev_t;
        typedef int32_t blkcnt_t;
        typedef int32_t blksize_t;

        #define OFF_T_MAX INT32_MAX
        #define OFF_T_MIN INT32_MIN
    #endif

    typedef uint32_t mode_t;
    typedef int32_t  nlink_t;
    typedef int32_t  uid_t;
    typedef int32_t  gid_t;

    typedef intptr_t ssize_t;
    #define SSIZE_MAX INTPTR_MAX

#else // NXIO_POSIX

    typedef int32_t off_t;
    typedef int32_t ino_t;
    typedef int32_t dev_t;
    typedef int32_t blkcnt_t;
    typedef int32_t blksize_t;

    typedef uint16_t mode_t;
    typedef int16_t  nlink_t;
    typedef int32_t  uid_t;
    typedef int32_t  gid_t;

    #define OFF_T_MAX INT32_MAX
    #define OFF_T_MIN INT32_MIN

#endif // NXIO_POSIX
