#include <windows.h>

#include "nxio/errno_win32.h"

#include "io.h"
#include "include/unistd.h"
#include "errno.h"

int nxio_access(const char *path, int amode)
{
    if (!path) {
        errno = EINVAL;
        return -1;
    }

    DWORD attr = GetFileAttributesA(path);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        nxcrt_errno_win32(GetLastError());
        return -1;
    }

    if (amode == NX_F_OK)
        return 0;

    // Best effort
    if ((amode & NX_W_OK) &&
        (attr & FILE_ATTRIBUTE_READONLY) &&
        !(attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        errno = EACCES;
        return -1;
    }

    // Everything else? Should be good.
    return 0;
}
