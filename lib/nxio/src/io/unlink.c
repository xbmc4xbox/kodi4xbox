#include <windows.h>

#include "nxio/errno_win32.h"

#include "io.h"
#include "unistd.h"
#include "errno.h"

int nxio_unlink(const char *path)
{
    if (!path) {
        errno = EINVAL;
        return -1;
    }

    DWORD attrs = GetFileAttributesA(path);

    if (attrs == INVALID_FILE_ATTRIBUTES) {
        nxcrt_errno_win32(GetLastError());
        return -1;
    }

    if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
        errno = EISDIR;
        return -1;
    }

    if (!DeleteFileA(path)) {
        nxcrt_errno_win32(GetLastError());
        return -1;
    }

    return 0;
}
