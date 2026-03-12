#include <windows.h>
#include <time.h>

#include "nxio/fd.h"
#include "nxio/lk.h"

#include "errno.h"
#include "sys/stat.h"
#include "fcntl.h"

static int nxio_fstat_internal(int fd, struct nx_stat64_internal *buf)
{
    if (!buf) {
        errno = EINVAL;
        return -1;
    }

    nxcrt_file_t *file = nxcrt_fd_get(fd);

    if (!file)
        return -1;

    nxcrt_file_lock(file);

    int ret = nxio_fstat_lk(file, buf);

    nxcrt_file_unlock(file);
    nxcrt_fd_release(fd);

    return ret;
}

#ifdef NXIO_POSIX

static void nxcrt_internal_to_posix(
    const struct nx_stat64_internal *in,
    struct stat *out)
{
    out->st_dev   = (dev_t)in->st_dev;
    out->st_ino   = (ino_t)in->st_ino;
    out->st_mode  = (mode_t)in->st_mode;
    out->st_nlink = (nlink_t)in->st_nlink;
    out->st_uid   = (uid_t)in->st_uid;
    out->st_gid   = (gid_t)in->st_gid;
    out->st_rdev  = (dev_t)in->st_rdev;
    out->st_size  = (off_t)in->st_size;
    out->st_atime = (time_t)in->st_atime;
    out->st_mtime = (time_t)in->st_mtime;
    out->st_ctime = (time_t)in->st_ctime;
}

int fstat(int fd, struct stat *buf)
{
    struct nx_stat64_internal tmp;
    int ret = nxio_fstat_internal(fd, &tmp);
    if (ret == 0)
        nxcrt_internal_to_posix(&tmp, buf);
    return ret;
}

int stat(const char *path, struct stat *buf)
{
    if (!path || !buf) {
        errno = EINVAL;
        return -1;
    }

    int fd = nxio_sopen_impl(path, NX_O_RDONLY, NX_SH_DENYNO, 0);
    if (fd < 0) {
        return -1;
    }

    int ret = fstat(fd, buf);

    nxio_close(fd);

    return ret;
}

#else // NXIO_POSIX

static void nxcrt_internal_to_stat32(
    const struct nx_stat64_internal *in,
    struct _stat *out)
{
    out->st_dev   = (int32_t)in->st_dev;
    out->st_ino   = (int32_t)in->st_ino;
    out->st_mode  = (int16_t)in->st_mode;
    out->st_nlink = (int16_t)in->st_nlink;
    out->st_uid   = (int32_t)in->st_uid;
    out->st_gid   = (int32_t)in->st_gid;
    out->st_rdev  = (int32_t)in->st_rdev;
    out->st_size  = (int32_t)in->st_size;
    out->st_atime = (int32_t)in->st_atime;
    out->st_mtime = (int32_t)in->st_mtime;
    out->st_ctime = (int32_t)in->st_ctime;
}

static void nxcrt_internal_to_stati64(
    const struct nx_stat64_internal *in,
    struct _stati64 *out)
{
    out->st_dev   = (int32_t)in->st_dev;
    out->st_ino   = (int64_t)in->st_ino;
    out->st_mode  = (int16_t)in->st_mode;
    out->st_nlink = (int16_t)in->st_nlink;
    out->st_uid   = (int32_t)in->st_uid;
    out->st_gid   = (int32_t)in->st_gid;
    out->st_rdev  = (int32_t)in->st_rdev;
    out->st_size  = (int64_t)in->st_size;
    out->st_atime = (int32_t)in->st_atime;
    out->st_mtime = (int32_t)in->st_mtime;
    out->st_ctime = (int32_t)in->st_ctime;
}

static void nxcrt_internal_to_stat64(
    const struct nx_stat64_internal *in,
    struct _stat64 *out)
{
    out->st_dev   = (int32_t)in->st_dev;
    out->st_ino   = (int64_t)in->st_ino;
    out->st_mode  = (int16_t)in->st_mode;
    out->st_nlink = (int16_t)in->st_nlink;
    out->st_uid   = (int32_t)in->st_uid;
    out->st_gid   = (int32_t)in->st_gid;
    out->st_rdev  = (int32_t)in->st_rdev;
    out->st_size  = (int64_t)in->st_size;
    out->st_atime = (int64_t)in->st_atime;
    out->st_mtime = (int64_t)in->st_mtime;
    out->st_ctime = (int64_t)in->st_ctime;
}

int _fstat(int fd, struct _stat *buf)
{
    struct nx_stat64_internal tmp;
    int ret = nxio_fstat_internal(fd, &tmp);
    if (ret == 0)
        nxcrt_internal_to_stat32(&tmp, buf);
    return ret;
}

int _stat(const char *path, struct _stat *buf)
{
    if (!path || !buf) {
        errno = EINVAL;
        return -1;
    }

    int fd = nxio_sopen_impl(path, NX_O_RDONLY, NX_SH_DENYNO, 0);
    if (fd < 0) {
        return -1;
    }

    int ret = _fstat(fd, buf);

    nxio_close(fd);

    return ret;
}

int _fstati64(int fd, struct _stati64 *buf)
{
    struct nx_stat64_internal tmp;
    int ret = nxio_fstat_internal(fd, &tmp);
    if (ret == 0)
        nxcrt_internal_to_stati64(&tmp, buf);
    return ret;
}

int _stati64(const char *path, struct _stati64 *buf)
{
    if (!path || !buf) {
        errno = EINVAL;
        return -1;
    }

    int fd = nxio_sopen_impl(path, NX_O_RDONLY, NX_SH_DENYNO, 0);
    if (fd < 0) {
        return -1;
    }

    int ret = _fstati64(fd, buf);

    nxio_close(fd);

    return ret;
}

int _fstat64(int fd, struct _stat64 *buf)
{
    struct nx_stat64_internal tmp;
    int ret = nxio_fstat_internal(fd, &tmp);
    if (ret == 0)
        nxcrt_internal_to_stat64(&tmp, buf);
    return ret;
}

int _stat64(const char *path, struct _stat64 *buf)
{
    if (!path || !buf) {
        errno = EINVAL;
        return -1;
    }

    int fd = nxio_sopen_impl(path, NX_O_RDONLY, NX_SH_DENYNO, 0);
    if (fd < 0) {
        return -1;
    }

    int ret = _fstat64(fd, buf);

    nxio_close(fd);

    return ret;
}

#endif // NXIO_POSIX
