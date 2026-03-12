#include <xboxkrnl/xboxkrnl.h>
#include <stdio.h>
#include <stdarg.h>


void nxio_print_debug_impl(const char *fmt, ...) 
{
    char buffer[1024];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    DbgPrint("nxio: %s\n", buffer);
}