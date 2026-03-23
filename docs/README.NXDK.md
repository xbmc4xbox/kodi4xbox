# Original Xbox Build Guide

To build Kodi, we will need an open-source SDK known as **NXDK**. The official NXDK is missing support for C++ exceptions so we are going to use NXDK [fork](https://github.com/xbmc4xbox/nxdk.git) which contains WIP exceptions from thrimbor.

## Table of Contents
1. [Document Conventions](#1-document-conventions)
2. [Set Up NXDK SDK](#2-set-up-nxdk-sdk)
3. [Build Kodi](#3-build-kodi)

## 1. Document Conventions
This guide assumes you are familiar with `Linux`, the command line (`CLI`), and `Git`.
Commands should be run in the terminal, one at a time and in the order provided.
If you are on Windows, you can use WSL (Windows Subsystem for Linux).

## 2. Set Up SDK
Before we dive in, make sure you have both CMake and CLang installed. You can confirm that by running following commands:
```bash
cmake --version
clang --version
```
**IMPORTANT: clang version must be v20+!**

Now we are ready do setup SDK. The first thing you need to do is to clone NXDK:
```bash
git clone https://github.com/xbmc4xbox/nxdk.git
```
After cloning, switch to branch which contains support for C++ exceptions:
```bash
git checkout rtti_exceptions
```
Now pull all required submodules:
```bash
git submodule update --init --recursive
```
Now we will build C++ sample program to confirm if SDK is properly configured. Execute following commands one by one:
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
This means that SDK is properly configured and is ready for building Kodi. If there are any issues check out official [documentation](https://github.com/XboxDev/nxdk/wiki/Getting-Started).

Final step before building Kodi is to add `NXDK_DIR` system variable. To do that, add:
```bash
export NXDK_DIR="<path-to-nxdk>/nxdk"
```
to `.bash_profile`. Make sure to replace `<path-to-nxdk>` with your local path. Then restart your PC and when you type `printenv NXD_DIR` in terminal you should get output similar to this:
```bash
/<path-to-nxdk>/nxdk
```
And now we can finally build Kodi!

## 3. Build Kodi
Clone the Kodi repository:
```bash
git clone https://github.com/xbmc4xbox/kodi4xbox.git
```
Navigate into the Kodi directory:
```bash
cd kodi4xbox
```
Create and enter build folder:
```bash
mkdir build && cd build
```
Now run cmake to in order to download and configure needed libraries:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=${NXDK_DIR}/share/toolchain-nxdk.cmake -DCMAKE_BUILD_TYPE=Release
```
Finally, compile Kodi with:
```bash
cmake --build . --parallel $(nproc)
```
If everything went well you should have Xbox executable inside `build` folder called `default.xbe`

## How to Debug
- First we need to build NXDK libs as Debug. To do that build hello++ example with `DEBUG = y` added to `samples/hello++/Makefile`
- Debug executable will be quite big because of symbols. Before building apply [this](https://github.com/xbmc4xbox/kodi4xbox/blob/master/docs/resources/debug_kodi.patch) patch:
  ```bash
  curl -L https://raw.githubusercontent.com/xbmc4xbox/kodi4xbox/master/docs/resources/debug_kodi.patch | git apply
  ```
- Build Kodi in same way as described above, but make sure to build Debug version. You do that by setting `-DCMAKE_BUILD_TYPE` to `Debug`
- After succesfull build, open Xemu, load ISO from build folder and then close Xemu. After that, start debugging from within Visual Studio Code by pressing `CTRL+SHIFT+D` and then `F5`
