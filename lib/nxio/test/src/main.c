#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <nxdk/mount.h>
#include <nxdk/path.h>
#include "test.h"

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    debugPrint("Starting up\n");

    if (nxIsDriveMounted('D'))
    {
        nxUnmountDrive('D');
    }

    // Mount the DVD drive
    nxMountDrive('D', "\\Device\\CdRom0");

    // Mount root of LithiumX xbe to Q:
    char targetPath[MAX_PATH];
    nxGetCurrentXbeNtPath(targetPath);
    *(strrchr(targetPath, '\\') + 1) = '\0';
    nxMountDrive('Q', targetPath);

    // Mount stock partitions
    nxMountDrive('C', "\\Device\\Harddisk0\\Partition2\\");
    nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    nxMountDrive('X', "\\Device\\Harddisk0\\Partition3\\");
    nxMountDrive('Y', "\\Device\\Harddisk0\\Partition4\\");
    nxMountDrive('Z', "\\Device\\Harddisk0\\Partition5\\");

    nxMountDrive('F', "\\Device\\Harddisk0\\Partition6\\");
    nxMountDrive('G', "\\Device\\Harddisk0\\Partition7\\");
    nxMountDrive('R', "\\Device\\Harddisk0\\Partition8\\");
    nxMountDrive('S', "\\Device\\Harddisk0\\Partition9\\");
    nxMountDrive('V', "\\Device\\Harddisk0\\Partition10\\");
    nxMountDrive('W', "\\Device\\Harddisk0\\Partition11\\");
    nxMountDrive('A', "\\Device\\Harddisk0\\Partition12\\");
    nxMountDrive('B', "\\Device\\Harddisk0\\Partition13\\");
    nxMountDrive('P', "\\Device\\Harddisk0\\Partition14\\");

    debugPrint("Partitions mounted\n");

    nxio_run_tests();

    while(1) {
        Sleep(2000);
    }

    return 0;
}
