#include <windows.h>

#include "nxio/lk.h"
#include "nxio/errno_win32.h"

#include "errno.h"
#include "sys/stat.h"

static int64_t nxcrt_filetime_to_unix(const FILETIME *ft)
{
    if (!ft)
        return 0;

    ULARGE_INTEGER ull;
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;

    if (ull.QuadPart == 0 || ull.QuadPart < 116444736000000000ULL)
        return 0;

    // Convert from 100ns intervals to seconds
    unsigned long long seconds = ull.QuadPart / 10000000ULL;

    // Convert Windows epoch (1601) to Unix epoch (1970)
    seconds -= 11644473600ULL;

    return (int64_t)seconds;
}

int nxio_fstat_lk(nxcrt_file_t *file, struct nx_stat64_internal *buf)
{
    if (file->type != NXCRT_FD_TYPE_WIN32) {
        errno = EBADF;
        return -1;
    }

    HANDLE h = (HANDLE)file->handle;
    if (h == INVALID_HANDLE_VALUE) {
        errno = EBADF;
        return -1;
    }

    BY_HANDLE_FILE_INFORMATION bhfi;

    if (!GetFileInformationByHandle(h, &bhfi)) {
        nxcrt_errno_win32(GetLastError());
        return -1;
    }

    memset(buf, 0, sizeof(*buf));

    buf->st_rdev = 0;
    buf->st_uid = 0;
    buf->st_gid = 0;
    buf->st_dev = bhfi.dwVolumeSerialNumber;
    buf->st_ino = ((int64_t)bhfi.nFileIndexHigh << 32) | (int64_t)bhfi.nFileIndexLow;
    buf->st_nlink = bhfi.nNumberOfLinks;

    buf->st_size = ((int64_t)bhfi.nFileSizeHigh << 32) | (int64_t)bhfi.nFileSizeLow;

    // Mode
    if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        buf->st_mode = _S_IFDIR;
    else
        buf->st_mode = _S_IFREG;

    if (bhfi.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        buf->st_mode |= _S_IREAD;
    else
        buf->st_mode |= (_S_IREAD | _S_IWRITE);

    // Time conversion
    buf->st_mtime = nxcrt_filetime_to_unix(&bhfi.ftLastWriteTime);

    if (bhfi.ftLastAccessTime.dwLowDateTime || bhfi.ftLastAccessTime.dwHighDateTime)
        buf->st_atime = nxcrt_filetime_to_unix(&bhfi.ftLastAccessTime);
    else
        buf->st_atime = buf->st_mtime;

    if (bhfi.ftCreationTime.dwLowDateTime || bhfi.ftCreationTime.dwHighDateTime)
        buf->st_ctime = nxcrt_filetime_to_unix(&bhfi.ftCreationTime);
    else
        buf->st_ctime = buf->st_mtime;

    return 0;
}