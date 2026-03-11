<p align="center">
  <img src="media/splash.jpg" alt="Kodi Logo"/>
</p>

## Kodi
This is port of Kodi to Original Xbox using open source toolchain known as [NXDK (New Xbox Development Kit)](https://github.com/XboxDev/nxdk). It's currently in pre-alpha stage. I will need help from other devs to get it working, so contributions are welcome!

## Download links
This software is in alpha and currently we are only providing nightly releases.

ISO (XEMU) - [Download Link](https://github.com/antonic901/xbmc4xbox-nxdk/releases/download/nightly/Kodi.iso)
<br/>
ZIP (XBOX) - [Download Link](https://github.com/antonic901/xbmc4xbox-nxdk/releases/download/nightly/Kodi.zip)

## How to install
If you want to try in XEMU download and use ISO. If you want to try on real hardware download ZIP, extract it and move Kodi folder somewhere to E partition. Do not install on extended partitions like F,G etc. it won't work!

## Current status
We are targeting Kodi v21 Omega. App is compiling and can be run however it's not ready to be usead as main dashboard. Lot of things is either broken or partially working:

- memory usage is catastrophic compared to XBMC 4.0
- lot of memory leaks
- missing video playback
- missing audio playback
- no UI sounds
- no support for binary addons (python, visualizations, screensavers)
- no support for compressed textures
- joystick input is partially implemented and has lot of problems
- pbkit/pbgl which are used to implement renderer are not great and will need lot of improvements (planty of wasted memory compared to XDK / DirectX)
- can be unstable because of WIP C++ exceptions and NXDK issues

For more details, please check our [roadmap](docs/roadmap/README.md).
