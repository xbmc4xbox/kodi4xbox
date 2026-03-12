#include <windows.h>
#include <stdio.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "fcntl.h"
#include "errno.h"

int nxio_read_lk(nxcrt_file_t *file, void *buf, unsigned int cnt)
{
    if (cnt == 0 || (file->status_flags & NXCRT_FEOFLAG))
        return 0;

    if (file->type != NXCRT_FD_TYPE_WIN32) {
        errno = EBADF;
        return -1;
    }

    HANDLE h = (HANDLE)file->handle;
    if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    char *buffer = buf;
    DWORD os_read = 0;

    if (!ReadFile(h, buffer, (DWORD)cnt, &os_read, NULL)) {
        DWORD err = GetLastError();

        if (err == ERROR_BROKEN_PIPE)
            return 0;

        if (err == ERROR_ACCESS_DENIED)
        {
            errno = EBADF;
            return -1;
        }

        nxcrt_errno_win32(err);
        return -1;
    }

    file->offset += os_read;

    if (!(file->flags & NX_O_TEXT))
        return (int)os_read;

    char *p = buffer;
    char *q = buffer;
    char *end = buffer + os_read;

    while (p < end) {

        if (*p == 26) {
            file->status_flags |= NXCRT_FEOFLAG;
            break;
        }

        if (*p != '\r') {
            *q++ = *p++;
        }
        else {
            if (p + 1 < end) {
                if (*(p + 1) == '\n') {
                    p += 2;
                    *q++ = '\n';
                } else {
                    *q++ = *p++;
                }
            }
            else {
                char peek;
                DWORD peek_read = 0;

                if (!ReadFile(h, &peek, 1, &peek_read, NULL) || peek_read == 0)
                {
                    *q++ = '\r';
                }
                else {
                    if (peek == '\n') {
                        *q++ = '\n';
                    } else {
                        nxio_lseek_lk(file, -1, SEEK_CUR);
                        *q++ = '\r';
                    }
                }

                p++;
            }
        }
    }

    return (int)(q - (char *)buf);
}
