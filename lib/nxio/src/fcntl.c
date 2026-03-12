#include <stdarg.h>
#include <errno.h>

#include "nxio/fd.h"

#include "fcntl.h"

// Experimental. Not part of MSVC.
int nxio_fcntl(int fd, int cmd, ...)
{
    int result = -1;
    va_list ap;

    if (fd < 0 || fd >= NXCRT_MAX_FDS) {
        errno = EBADF;
        return -1;
    }

    EnterCriticalSection(&nxcrt_fd_table_lock);

    if (!nxcrt_fd_table[fd].used || !nxcrt_fd_table[fd].file) {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        errno = EBADF;
        return -1;
    }

    nxcrt_fd_t *fde = &nxcrt_fd_table[fd];
    nxcrt_file_t *file = fde->file;

    switch (cmd) {
        case NX_F_DUPFD:
        {
            va_start(ap, cmd);
            int minfd = va_arg(ap, int);
            va_end(ap);

            if (minfd < 0 || minfd >= NXCRT_MAX_FDS) {
                errno = EINVAL;
                break;
            }

            for (int i = minfd; i < NXCRT_MAX_FDS; ++i) {
                if (!nxcrt_fd_table[i].used) {

                    nxcrt_fd_table[i].used = 1;
                    nxcrt_fd_table[i].file = file;
                    nxcrt_fd_table[i].fd_flags = fde->fd_flags;

                    nxcrt_file_lock(file);
                    file->refcount++;
                    nxcrt_file_unlock(file);

                    result = i;
                    break;
                }
            }

            if (result == -1)
                errno = EMFILE;

            break;
        }

        /* Descriptor flags */
        case NX_F_GETFD:
            result = fde->fd_flags & NX_FD_CLOEXEC;
            break;

        case NX_F_SETFD:
        {
            va_start(ap, cmd);
            int flags = va_arg(ap, int);
            va_end(ap);

            fde->fd_flags = flags & NX_FD_CLOEXEC;
            result = 0;
            break;
        }

        /* File status flags */
        case NX_F_GETFL:
            int visible =
                NX_O_ACCMODE |
                NX_O_APPEND |
                NX_O_NONBLOCK |
                NX_O_SYNC |
                NX_O_DSYNC |
                NX_O_RSYNC |
                NX_O_ASYNC |
                NX_O_TEXT;

            nxcrt_file_lock(file);
            result = file->flags & visible;
            nxcrt_file_unlock(file);
            break;

        case NX_F_SETFL:
        {
            va_start(ap, cmd);
            int flags = va_arg(ap, int);
            va_end(ap);

            int changeable = NX_O_APPEND | NX_O_NONBLOCK | NX_O_ASYNC;

            nxcrt_file_lock(file);
            file->flags &= ~changeable;
            file->flags |= (flags & changeable);

            nxcrt_file_unlock(file);

            result = 0;
            break;
        }

        default:
            errno = EINVAL;
            break;
    }

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    return result;
}
