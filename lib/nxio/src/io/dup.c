#include <stdlib.h>

#include "nxio/fd.h"

#include "io.h"
#include "unistd.h"
#include "errno.h"

int nxio_dup(int oldfd)
{
    if (oldfd < 0 || oldfd >= NXCRT_MAX_FDS)
    {
        errno = EBADF;
        return -1;
    }

    EnterCriticalSection(&nxcrt_fd_table_lock);

    if (!nxcrt_fd_table[oldfd].used ||
        !nxcrt_fd_table[oldfd].file)
    {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        errno = EBADF;
        return -1;
    }

    nxcrt_file_t *file = nxcrt_fd_table[oldfd].file;

    int newfd = -1;

    for (int i = 3; i < NXCRT_MAX_FDS; ++i)
    {
        if (!nxcrt_fd_table[i].used)
        {
            newfd = i;
            nxcrt_fd_table[i].used = 1;
            nxcrt_fd_table[i].file = file;

            EnterCriticalSection(&file->lock);
            file->refcount++;
            LeaveCriticalSection(&file->lock);

            break;
        }
    }

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    if (newfd < 0)
        errno = EMFILE;

    return newfd;
}

int nxio_dup2(int oldfd, int newfd)
{
    if (oldfd < 0 || oldfd >= NXCRT_MAX_FDS || newfd < 0 || newfd >= NXCRT_MAX_FDS)
    {
        errno = EBADF;
        return -1;
    }

    EnterCriticalSection(&nxcrt_fd_table_lock);

    if (!nxcrt_fd_table[oldfd].used ||
        !nxcrt_fd_table[oldfd].file)
    {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        errno = EBADF;
        return -1;
    }

    if (oldfd == newfd)
    {
        LeaveCriticalSection(&nxcrt_fd_table_lock);
        return newfd;
    }

    nxcrt_file_t *newfile = nxcrt_fd_table[oldfd].file;
    nxcrt_file_t *oldfile = NULL;

    if (nxcrt_fd_table[newfd].used)
        oldfile = nxcrt_fd_table[newfd].file;

    EnterCriticalSection(&newfile->lock);
    newfile->refcount++;
    LeaveCriticalSection(&newfile->lock);

    nxcrt_fd_table[newfd].used = 1;
    nxcrt_fd_table[newfd].file = newfile;

    LeaveCriticalSection(&nxcrt_fd_table_lock);

    int should_close = 0;

    if (oldfile)
    {
        EnterCriticalSection(&oldfile->lock);
        oldfile->refcount--;
        should_close = (oldfile->refcount == 0);
        LeaveCriticalSection(&oldfile->lock);
    }

    if (should_close)
    {
        nxcrt_fd_close_underlying(oldfile);
        DeleteCriticalSection(&oldfile->lock);
        free(oldfile);
    }

    return newfd;
}