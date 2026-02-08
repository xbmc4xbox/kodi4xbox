# Original Xbox Build Guide

To build XBMC, we will need an open-source toolchain known as **NXDK**. The official NXDK is missing support for C++ exceptions so we are going to use NXDK fork which contains WIP exceptions from [thrimbor](https://github.com/thrimbor/nxdk/tree/cpp_exceptions_final).

## Table of Contents
1. [Document Conventions](#1-document-conventions)
2. [Set Up NXDK Toolchain](#2-set-up-nxdk-toolchain)
3. [Build XBMC](#3-build-xbmc)

## 1. Document Conventions
This guide assumes you are familiar with `Linux`, the command line (`CLI`), and `Git`.
Commands should be run in the terminal, one at a time and in the order provided.
If you are on Windows, you can use WSL (Windows Subsystem for Linux).

## 2. Set Up NXDK Toolchain
Before we dive in, make sure you have both CMake and CLang installed. You can confirm that by running following commands:
```bash
cmake --version
clang --version
```
**IMPORTANT: clang version must be v20+!**

Now we are ready do setup NXDL toolchain. The first thing you need to do is to clone NXDK:
```bash
git clone --recurse-submodules https://github.com/antonic901/nxdk.git
```
After cloning, switch to branch which contains support for C++ exceptions:
```bash
git checkout thrimbor_exceptions
```
Now we will build C++ sample program to confirm if building is working. Execute following commands one by one:
```bash
cd samples/hello++/
eval "$(../../bin/activate -s)"
make
```
If everything went well, you should see something similar to this:
```bash
[ LD       ] main.exe
lld: warning: .edata=.rdata: already merged into .edataxb
[ CXBE     ] bin/default.xbe
[ XISO     ] nxdk sample - hello++.iso
```
This means that NXDK is properly configured and is ready for building XBMC. If there are any issues check out official [documentation](https://github.com/XboxDev/nxdk/wiki/Getting-Started).

Final step before building XBMC is to add `NXDK_DIR` system variable. To do that, add:
```bash
export NXDK_DIR="<path-to-nxdk>/nxdk"
```
to `.bash_profile`. Make sure to replace `<path-to-nxdk>` with your local path. Then restart your PC and when you type `printenv NXD_DIR` in terminal you should get output similar to this:
```bash
/<path-to-nxdk>/nxdk
```
And now we can finally build XBMC!

## 3. Build XBMC
You can build XBMC with automated script which will create `dist` folder ready to be FTPed on your Xbox or you can build it manually.

## 3.1 Automatic build
Clone the XBMC repository:
```bash
git clone https://github.com/antonic901/xbmc4xbox-nxdk
```
Navigate into the XBMC directory:
```bash
cd xbmc4xbox-nxdk
```
Run build script:
```bash
./build.sh
```

## 3.2 Manual build
Clone the XBMC repository:
```bash
git clone https://github.com/antonic901/xbmc4xbox-nxdk
```
Navigate into the XBMC directory:
```bash
cd xbmc4xbox-nxdk
```
Create and enter build folder:
```bash
mkdir build && cd build
```
Now run cmake to in order to download and configure needed libraries:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=${NXDK_DIR}/share/toolchain-nxdk.cmake -DCMAKE_BUILD_TYPE=Release
```
Finally, compile XBMC with:
```bash
cmake --build .
```
If everything went well you should have Xbox executable inside `build` folder called `default.xbe`

## How to Debug
- First download [this](https://github.com/antonic901/xbmc4xbox-nxdk/blob/master/docs/resources/FFXXFFXX.zip) archive
- Extract archive and copy folder FFXXFFXX to your Xbox using FTP. Place it inside `E:\UDATA\`
- Build Kodi in same way as described above, but make sure to build Debug version, not Release!
- After succesfull build, open Xemu, load ISO from build folder and then close Xemu. After that, start debugging from within Visual Studio Code by pressing `CTRL+SHIFT+D` and then `F5`
