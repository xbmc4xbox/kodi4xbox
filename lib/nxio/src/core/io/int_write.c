#include <windows.h>
#include <stdio.h>

#include "nxio/fd.h"
#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "io.h"
#include "fcntl.h"
#include "errno.h"

int nxio_write_lk(nxcrt_file_t *file, const void *buf, unsigned int cnt)
{
    if (cnt == 0)
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

    if (file->flags & NX_O_APPEND) {
        if (nxio_lseek_lk(file, (off_t)0, SEEK_END) < 0)
            return -1;
    }

    if (!(file->flags & NX_O_TEXT)) { // _O_TEXT

        DWORD written = 0;

        if (!WriteFile(h, buf, (DWORD)cnt, &written, NULL)) {
            nxcrt_errno_win32(GetLastError());
            return -1;
        }

        // 0-byte write is valid, do NOT set ENOSPC
        file->offset += written;

        return (int)written;
    }

    const char *p = (const char *)buf;
    unsigned int remaining = cnt;
    int logical_written = 0;

    char lfbuf[1024];

    while (remaining > 0) {

        char *q = lfbuf;
        unsigned int produced = 0;
        unsigned int consumed = 0;

        while (produced < sizeof(lfbuf) - 1 && remaining > 0) {
            char ch = *p++;
            remaining--;
            consumed++;

            if (ch == '\n') {
                *q++ = '\r';
                produced++;
            }

            *q++ = ch;
            produced++;
        }

        DWORD chunk_written = 0;

        if (!WriteFile(h, lfbuf, (DWORD)produced, &chunk_written, NULL)) {
            nxcrt_errno_win32(GetLastError());
            return -1;
        }

        if (chunk_written == 0) {
            file->offset += chunk_written;
            return logical_written;
        }

        file->offset += chunk_written;

        // Count logical bytes written
        for (DWORD i = 0; i < chunk_written; ++i) {
            if (lfbuf[i] != '\r')
                logical_written++;
        }
    }

    return logical_written;
}
