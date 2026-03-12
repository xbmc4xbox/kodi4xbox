cmake_minimum_required(VERSION 3.18)
project(nxio LANGUAGES C)

add_library(nxio
  src/core/errno.c
  src/core/errno_win32.c
  src/core/fd.c
  src/core/io/int_lseek.c
  src/core/io/int_read.c
  src/core/io/int_stat.c
  src/core/io/int_write.c
  src/fcntl.c
  src/io/access.c
  src/io/close.c
  src/io/dup.c
  src/io/lseek.c
  src/io/open.c
  src/io/read.c
  src/io/stat.c
  src/io/unlink.c
  src/io/write.c
  src/util/log.c
)

target_include_directories(nxio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
