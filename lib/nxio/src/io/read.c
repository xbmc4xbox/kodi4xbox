#include <windows.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "unistd.h"
#include "fcntl.h"
#include "errno.h"

static int nxio_read_internal(int fd, void *buf, unsigned int cnt)
{
    nxcrt_file_t *file = nxcrt_fd_get(fd);
    if (!file)
        return -1;
    
    int acc = file->flags & NX_O_ACCMODE;
    if (acc != NX_O_RDONLY && acc != NX_O_RDWR) {
        errno = EBADF;
        nxcrt_fd_release(fd);
        return -1;
    }

    nxcrt_file_lock(file);

    int ret = nxio_read_lk(file, buf, cnt);

    nxcrt_file_unlock(file);
    nxcrt_fd_release(fd);

    return ret;
}

#ifdef NXIO_POSIX

ssize_t nxio_read_posix(int fd, void *buf, size_t cnt) {
    if (cnt > UINT_MAX) {
        errno = EINVAL;
        return -1;
    }

    int ret = nxio_read_internal(fd, buf, (unsigned int) cnt);

    if (ret < 0)
        return -1;

    return (ssize_t)ret;
}

#else // NXIO_POSIX

int nxio_read32(int fd, void *buf, unsigned int cnt) {
    return nxio_read_internal(fd, buf, cnt);
}

#endif // NXIO_POSIX
