## Introduction
Before we dive into the roadmap, let me give you a bit of background on my involvement with Kodi and XBMC. In late 2022, I started looking into the source code of XBMC4Xbox, hoping to update GUILIB to make the latest Estuary skin compatible and functional in XBMC4Xbox. I've been working on this for the past two and a half years, and in addition to GUILIB, I also backported the Addons Framework, Media Libraries, and updated almost every part of XBMC4Xbox to align with Kodi Krypton. Some of my work can be found on my [YouTube channel](https://www.youtube.com/@nikolaantonic132/videos). A release date, along with the source code, is planned for this year, though no precise date has been set yet.

Thanks to all of this, I wanted to push even further and bring the source code up to a more modern Kodi. However, C++98 from official XDK was holding me back, and with each newer release of Kodi, it became harder and harder to get the code working and compiling. Since I now have a good overview of every part of Kodi, I decided to try bringing XBMC4Xbox/Kodi to the Original Xbox but this time by using the open-source toolchain NXDK, which supports C++17 and reimplements functions from newer versions of Windows—such as ConditionVariable, for example.

I started line by line, file by file, and eventually, I was able to compile Kodi Nexus. However audio, input, network, dll loading and video rendering are still not implemented. These areas which involves good understanding of computer graphics and low-level debugging aren't my strong sides, so I’m releasing the source code in hopes that other developers will join me on this adventure and help get XBMC fully working with NXDK.

## Goals
The first goal is basically getting **GUILIB** working properly. That includes proper texture loading and rendering.

## Milestones

### Milestone 0
Milestone 0 (zero) should add missing features to NXDK which are required by XBMC. File [unimplemented.h](https://github.com/antonic901/xbmc4xbox-nxdk/blob/master/xbmc/unimplemented.h) contains mock functions which are still not implemented in NXDK but are required by XBMC.

* [ ] Missing support for **RTTI**. Stuff like exceptions, dynamic_cast and typeid wont' work
* [ ] Missing support for widechars. Stuff like WideCharToMultiByte, MultiByteToWideChar, vswprintf, wcstombs etc. won't work
* [ ] Missing **WINAPI** functions `LocalFileTimeToFileTime`, `FlushFileBuffers`
* [ ] Missing support for float numbers. Stuff like `atof`, `strtod`, `strtof` etc. won't work

### Milestone 1
Milestone 1 (one) is dealing with GUILIB and everything related to it. That includes:
* [x] Texture loading -> implementation of CTexture which is used for loading textures (libjpeg-turbo/libpng). Example from XBMC4Xbox is [here](https://github.com/antonic901/xbmc4xbox-redux/blob/master/xbmc/guilib/Texture.h) - added with [0a5a7e22](https://github.com/antonic901/xbmc4xbox-nxdk/commit/0a5a7e22b19a2c2f970e0af39909b800dd9706e9)
* [x] Texture rendering -> implementation of CGUITexturePBKIT which is used for texture rendering. Example from XBMC4Xbox is [here](https://github.com/antonic901/xbmc4xbox-redux/blob/master/xbmc/guilib/GUITextureD3D.cpp) - added with [0a5a7e22](https://github.com/antonic901/xbmc4xbox-nxdk/commit/0a5a7e22b19a2c2f970e0af39909b800dd9706e9)
* [x] Properly initialize GPU device and everything related to it -> That's implementation of CGraphicContext
* [x] Properly create and initialize application -> Implement CApplication::Create(...), CApplication::Initialize(...), CApplication::Render(...), CApplication::Process(...) etc.
* [x] Font rendering - implementation of CGUIFontTTF, CGUIFont and CGUIFontManager. `freetype` lib is required for this - added with [0a5a7e22](https://github.com/antonic901/xbmc4xbox-nxdk/commit/0a5a7e22b19a2c2f970e0af39909b800dd9706e9)
* [x] Get splash screen properly rendering on screen
* [x] Get Home window properly rendering 

**IMPORTANT:** Texture loading and rendering was added with https://github.com/antonic901/xbmc4xbox-nxdk/pull/17. Read description of that PR to learn more!

**FOR DEVS: to quickly test CGraphicContext, texture loading and texture rendering we can use CSplash. CSplash is using texture loader and graphic context to load and render splash image. If that doesn't work then everything else regarding rendering and textures won't work.**

### Milestone 2
Milestone 2 (two) should add support for basic joypads and audio.

* [ ] UI audio - implement CGUIAudioManager and CAudioContext which is used for GUI audio actions which are WMA files
* [ ] Joypad input - implement basic support for input devices, aka joypads -> check `CInputManager::ProcessPeripherals`

### Milestone 3
Milestone 3 (three) should add support for reading HDD, partitions, free space, DVD drive support etc.

* [ ] CXboxProvider - implement of IStorageProvider interface which is used to read removable drives, HDD partitions, free space etc.
* [ ] DVD Drive support - reading inserted CDs and information about them -> requires `libcdio`

### Milestone 4
Milestone 4 (four) should add support for general Network and support for remote VFS paths (http/https).
* [ ] Basic network support - support for LAN connection
* [ ] Remote VFS paths - this one requires `libcurl` which is used for reading remote VFS paths (ex. `https://path/to/file`)

### Milestone 5
Milestone 5 (five) should add missing libraries which are required by XBMC but they are still not backported.

* [ ] [UnRARX](https://github.com/antonic901/xbmc4xbox-redux/tree/master/lib/UnrarXLib) - support for ZIP/RAR archives inside Kodi VFS
* [ ] [libcurl](https://github.com/antonic901/xbmc4xbox-redux/tree/master/lib/libcurl) - support for HTTP and other network protocols in Kodi VFS
* [x] [freetype](https://github.com/antonic901/xbmc4xbox-redux/tree/master/lib/freetype) - support for fonts (CGUIFontTTF, CGUIFont and CGUIFontManager) - added with [0a5a7e22](https://github.com/antonic901/xbmc4xbox-nxdk/commit/0a5a7e22b19a2c2f970e0af39909b800dd9706e9)
* [ ] [libcdio](https://github.com/antonic901/xbmc4xbox-redux/blob/master/docs/libcdio.rar) - used for reading DVD information
* [x] [DllLoader](https://github.com/antonic901/xbmc4xbox-redux/tree/master/xbmc/cores/DllLoader) - use for loading custom DLLs which were build using MinGW and Make. This version only support Windows 2K/XP compatible DLLs - adde with [779f7111](https://github.com/antonic901/xbmc4xbox-nxdk/commit/779f7111817896555bfb8d4cf3e0592e42d38b28)

### Milestone 6
At Milestone 6 (six) XBMC should be stable so we should start utilizing DllLoader and backporting Python and binary addons.

* [ ] Python Interpreter - support for Python scripts
* [ ] Binary addons - support for Screensavers, Visualizations, PVR etc.
* [ ] Move some libs to DLLs - to preserve some memory, libraries like libcurl should be build as DLL and loaded using DllLoader. This would keep them in memory only when we need them, but not all the time

### Milestone 7
Milestone 7 (seven) should add support for video and music playback. This one requires FFMpeg library because it is core of PAPlayer and DVDPlayer.

* [ ] FFMpeg - we need FFMpeg library for any media playback
* [ ] Video playback - backport of [DVDPlayer](https://github.com/antonic901/xbmc4xbox-redux/tree/master/xbmc/cores/dvdplayer) from XBMC4Xbox (VideoPlayer from Kodi is rebrand of DVDPlayer)
* [ ] Music playback - backport of [PAPlayer](https://github.com/antonic901/xbmc4xbox-redux/tree/master/xbmc/cores/paplayer) from XBMC4Xbox