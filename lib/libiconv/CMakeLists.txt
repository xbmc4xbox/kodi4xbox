cmake_minimum_required(VERSION 3.18)
project(libiconv LANGUAGES C)

add_library(libiconv libcharset/lib/localcharset.c lib/iconv.c lib/relocatable.c)

target_include_directories(libiconv
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/libcharset/include
)
