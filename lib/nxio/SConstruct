import os
import hashlib
import subprocess

env = Environment(
    tools=['clang', 'mslib'],
    CC='clang',
    AR='llvm-lib',
    LIBSUFFIX='.lib',
    NXDK_DIR=os.environ['NXDK_DIR'],
    ENV={
        'PATH': os.environ['PATH'],
    }
)

INTERMEDIATE_DIR = 'build/objs'

env['SHCCFLAGS'] = []
env['CCFLAGS'] = []
env['CFLAGS'] = []
env['LINKFLAGS'] = []
env['CPPPATH'] = []
env['CXXPATH'] = []
env['LIBS'] = []
env['LIBPATH'] = []

env.Append(CFLAGS=[
    '-target', 'i386-pc-win32',
    '-march=pentium3',
    '-fuse-ld=lld-link',
    '-ffreestanding',
    '-nostdinc',
    '-fno-builtin',
    '-Wno-builtin-macro-redefined',
    '-U__STDC_NO_THREADS__',

    '-O2',
#    '-flto',
    '-finline-functions'
])

env.Append(CPPDEFINES=[
    '_XBOX',
    'NXDK',
    '__STDC__=1',
])

env.Append(CPPPATH=[
    # Source includes
    'include',
    '$NXDK_DIR/lib/net/lwip/src/include',
    '$NXDK_DIR/lib/net/nforceif/include',
    '$NXDK_DIR/lib/net/nvnetdrv',
    '$NXDK_DIR/lib',
    '$NXDK_DIR/lib/xboxrt/libc_extensions',
    '$NXDK_DIR/lib/pdclib/include',
    '$NXDK_DIR/lib/pdclib/platform/xbox/include',
    '$NXDK_DIR/lib/winapi',
    '$NXDK_DIR/lib/xboxrt/vcruntime',
    os.path.join(subprocess.check_output(['clang', '-print-resource-dir'], text=True).strip(), 'include'),
])

# Sources
static_sources = [
    'src/core/errno_win32.c',
    'src/core/errno.c',
    'src/core/fd.c',
    'src/core/io/int_stat.c',
    'src/core/io/int_lseek.c',
    'src/core/io/int_read.c',
    'src/core/io/int_write.c',
    'src/util/log.c',
    'src/io/close.c',
    'src/io/dup.c',
    'src/io/open.c',
    'src/io/unlink.c',
    'src/io/access.c',
    'src/fcntl.c',
]

dynamic_sources = [
    'src/io/lseek.c',
    'src/io/read.c',
    'src/io/stat.c',
    'src/io/write.c',
]

posix_env = env.Clone()
posix_env.Append(CPPDEFINES=[
    'NXIO_POSIX',
    '_FILE_OFFSET_BITS=64',
])

objs = []

for src in static_sources:
    base = os.path.splitext(os.path.basename(src))[0]
    hash_suffix = hashlib.md5(src.encode()).hexdigest()[:8]
    obj_name = f'{base}_{hash_suffix}'
    obj = env.Object(target=os.path.join(INTERMEDIATE_DIR, obj_name), source=src)
    objs.append(obj)

for src in dynamic_sources:
    base = os.path.splitext(os.path.basename(src))[0]
    hash_suffix = hashlib.md5(src.encode()).hexdigest()[:8]

    for x in ["msvc", "posix"]:
        obj_name = f'{base}_{x}_{hash_suffix}'

        target_env = None
        if x == "msvc":
            target_env = env
        else:
            target_env = posix_env

        obj = target_env.Object(target=os.path.join(INTERMEDIATE_DIR, obj_name), source=src)
        objs.append(obj)

lib = env.Library(
    target='build/nxio.lib',
    source=objs,
)

oldnames = env.Command(
    target='build/nxoldnames.lib',
    source='src/nxoldnames.def',
    action='llvm-lib /def:$SOURCE /out:$TARGET /machine:i386'
)

env.Depends(lib, oldnames)

Default(lib)
