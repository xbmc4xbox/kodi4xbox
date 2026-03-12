# nxio

I/O abstraction layer extensions for NXDK. Provides implementations for standard headers like `io.h` and `sys/stat.h`, enabling easier porting of C applications and libraries to the original Xbox.

## Current features

- FD emulation
    - Provides reference counting, file flags, descriptor flags, offset tracking and sync
    - Fixed size. Adjust with `NXCRT_MAX_FDS`
- Thread-local `errno.h` implementation
    - Uses NXDK FLS to store POSIX `errno`
- Full Win32 error to errno conversion
- MSVC based POSIX headers
    - Provides the following POSIX headers. Keep in mind they are incomplete, check each one to see what is available
        - `<errno.h>`
        - `<fcntl.h>`
        - `<io.h>`
        - `<sys/stat.h>`
        - `<sys/types.h>`
- MSVC and POSIX personalities

## Limitations

- No std support
- No pipes support
- No devices support
- No async IO support
- No non-blocking IO support

## Building

- `NXIO_POSIX`: Define to `1` to declare POSIX personality. See also `_FILE_OFFSET_BITS`.
- `_FILE_OFFSET_BITS`: Define to `64` to use 64 bits file offsets.
- `NXCRT_MAX_FDS`: Size of the FD table. Defaults to `64`.
- `NXIO_DEBUG`: Define to `1` to enable serial debug output.
