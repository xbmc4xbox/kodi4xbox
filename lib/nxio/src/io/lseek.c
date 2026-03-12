#include <windows.h>
#include <limits.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "unistd.h"
#include "errno.h"
#include "sys/types.h"

static int64_t nxio_lseek_internal(int fd, int64_t offset, int whence)
{
    nxcrt_file_t *file = nxcrt_fd_get(fd);
    if (!file)
        return -1;

    nxcrt_file_lock(file);

    int64_t ret = nxio_lseek_lk(file, offset, whence);

    nxcrt_file_unlock(file);
    nxcrt_fd_release(fd);

    return ret;
}

#ifdef NXIO_POSIX
off_t nxio_lseek(int fd, off_t offset, int whence)
{
    int64_t result = nxio_lseek_internal(fd, (int64_t) offset, whence);

    if (result < 0)
        return -1;

    if (result > OFF_T_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    return (off_t)result;
}

#else // NXIO_POSIX

long nxio_lseek32(int fd, long offset, int whence)
{
    int64_t result = nxio_lseek_internal(fd, (int64_t) offset, whence);

    if (result < 0)
        return -1;

    if (result > LONG_MAX) {
        errno = EOVERFLOW;
        return -1;
    }

    return (long)result;
}

int64_t nxio_lseeki64(int fd, int64_t offset, int whence)
{
    int64_t result = nxio_lseek_internal(fd, offset, whence);

    if (result < 0)
        return -1;

    return result;
}

#endif // NXIO_POSIX
