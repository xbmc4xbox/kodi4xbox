#pragma once

// Keep in mind, not all flags are supported.

#ifdef __cplusplus
extern "C" {
#endif

// Access mode (bits 0–1)
#define NX_O_RDONLY        0x00000000
#define NX_O_WRONLY        0x00000001
#define NX_O_RDWR          0x00000002
#define NX_O_ACCMODE       0x00000003

// Creation flags (bits 8–15)
#define NX_O_CREAT         0x00000100
#define NX_O_EXCL          0x00000200
#define NX_O_NOCTTY        0x00000400
#define NX_O_TRUNC         0x00000800

// Status flags (bits 16–23)
#define NX_O_APPEND        0x00010000
#define NX_O_NONBLOCK      0x00020000
#define NX_O_SYNC          0x00040000
#define NX_O_DSYNC         0x00080000
#define NX_O_RSYNC         0x00100000
#define NX_O_DIRECTORY     0x00200000
#define NX_O_NOFOLLOW      0x00400000

// Behavior hints (POSIX optional)
#define NX_O_ASYNC         0x01000000

// MSVC extensions (top bits)
#define NX_O_RANDOM        0x10000000
#define NX_O_SEQUENTIAL    0x20000000
#define NX_O_TEMPORARY     0x40000000
#define NX_O_TEXT          0x80000000  /* text mode if not set */

// Share mode flags (MSVC only)
#define NX_SH_COMPAT     0x00
#define NX_SH_DENYRW     0x10
#define NX_SH_DENYWR     0x20
#define NX_SH_DENYRD     0x30
#define NX_SH_DENYNO     0x40

// FCNTL flags (POSIX only)
#define NX_F_DUPFD        0
#define NX_F_GETFD        1
#define NX_F_SETFD        2
#define NX_F_GETFL        3
#define NX_F_SETFL        4

// FD flags (POSIX only). This is not supported, but we keep it
// and assign it for compat reasons.
#define NX_FD_CLOEXEC        0x01

#ifdef NXIO_POSIX

    #define O_RDONLY       NX_O_RDONLY
    #define O_WRONLY       NX_O_WRONLY
    #define O_RDWR         NX_O_RDWR
    #define O_ACCMODE      NX_O_ACCMODE

    #define O_CREAT        NX_O_CREAT
    #define O_EXCL         NX_O_EXCL
    #define O_NOCTTY       NX_O_NOCTTY
    #define O_TRUNC        NX_O_TRUNC

    #define O_APPEND       NX_O_APPEND
    #define O_NONBLOCK     NX_O_NONBLOCK
    #define O_SYNC         NX_O_SYNC
    #define O_DSYNC        NX_O_DSYNC
    #define O_RSYNC        NX_O_RSYNC

    #define O_DIRECTORY    NX_O_DIRECTORY
    #define O_NOFOLLOW     NX_O_NOFOLLOW
    #define O_CLOEXEC      NX_O_CLOEXEC
    #define O_ASYNC        NX_O_ASYNC

    #define F_DUPFD        NX_F_DUPFD
    #define F_GETFD        NX_F_GETFD
    #define F_SETFD        NX_F_SETFD
    #define F_GETFL        NX_F_GETFL
    #define F_SETFL        NX_F_SETFL

    #define FD_CLOEXEC     NX_FD_CLOEXEC

    int nxio_fcntl(int fd, int cmd, ...);
    int nxio_open(const char *path, int oflag, ...);

    #define fcntl nxio_fcntl
    #define open nxio_open

#else /* NXIO_POSIX */

    #define _O_RDONLY      NX_O_RDONLY
    #define _O_WRONLY      NX_O_WRONLY
    #define _O_RDWR        NX_O_RDWR
    #define _O_ACCMODE     NX_O_ACCMODE

    #define _O_CREAT       NX_O_CREAT
    #define _O_EXCL        NX_O_EXCL
    #define _O_TRUNC       NX_O_TRUNC
    #define _O_APPEND      NX_O_APPEND

    #define _O_RANDOM      NX_O_RANDOM
    #define _O_SEQUENTIAL  NX_O_SEQUENTIAL
    #define _O_TEMPORARY   NX_O_TEMPORARY

    /* Text/binary mode */
    #define _O_TEXT     NX_O_TEXT
    #define _O_BINARY   0
    #define _O_RAW      0

    #define _SH_COMPAT     NX_SH_COMPAT
    #define _SH_DENYRW     NX_SH_DENYRW
    #define _SH_DENYWR     NX_SH_DENYWR
    #define _SH_DENYRD     NX_SH_DENYRD
    #define _SH_DENYNO     NX_SH_DENYNO

    /*
    * _open and _sopen are declared in <io.h>
    * fcntl is not a part of VC7 feature-set.
    */

#endif /* !NXIO_POSIX */

#ifdef __cplusplus
}
#endif
