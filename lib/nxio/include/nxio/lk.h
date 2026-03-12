#pragma once

#include "fd.h"

#include "sys/stat.h"
#include "sys/types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct nx_stat64_internal {
    int64_t  st_dev;
    int64_t  st_ino;
    uint32_t st_mode;
    int32_t  st_nlink;
    int32_t  st_uid;
    int32_t  st_gid;
    int64_t  st_rdev;
    int64_t  st_size;
    int64_t  st_atime;
    int64_t  st_mtime;
    int64_t  st_ctime;
};

int64_t nxio_lseek_lk(nxcrt_file_t *file, int64_t offset, int whence);
int nxio_read_lk(nxcrt_file_t *file, void *buf, unsigned int cnt);
int nxio_write_lk(nxcrt_file_t *file, const void *buf, unsigned int cnt);
int nxio_fstat_lk(nxcrt_file_t *file, struct nx_stat64_internal *buf);
int nxio_sopen_impl(const char *path, int oflag, int shflag, int pmode);
int nxio_close(int fd);

#ifdef __cplusplus
}
#endif
