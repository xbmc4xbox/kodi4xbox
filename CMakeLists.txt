cmake_minimum_required(VERSION 3.18)
set(XBE_TITLE xbmc)
set(XBOX_ISO_DIR ${CMAKE_CURRENT_BINARY_DIR}/xiso)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(NXDK_DEBUG_FLAG "y" CACHE STRING "Enable debug build for the NXDK submodule. 'y' or 'n'.")
endif()

find_package(Git REQUIRED)

project(xbmc C CXX ASM)

include(FetchContent)
include(FindPkgConfig)
include(PostbuildAction)
include(PrebuildNXDK)

set(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g -gdwarf-4 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -gdwarf-4 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -gdwarf-4 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -debug")

set(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -O2")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

add_compile_options(
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-gdwarf-4>
    $<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-gdwarf-4>
    -Wno-deprecated-declarations
    -Wno-inconsistent-missing-override
    -Wno-invalid-offsetof
)

add_definitions(-D_CRT_RAND_S -D_XBOX -DNXDK -DMEMP_NUM_NETBUF=6 -DMEMP_NUM_NETCONN=6 -DHAS_GL)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D_DEBUG)
endif()

add_executable(xbmc
  xbmc/AutoSwitch.cpp
  xbmc/BackgroundInfoLoader.cpp
  xbmc/CompileInfo.cpp
  xbmc/ContextMenuItem.cpp
  xbmc/ContextMenuManager.cpp
  xbmc/ContextMenus.cpp
  xbmc/CueDocument.cpp
  xbmc/DatabaseManager.cpp
  xbmc/DbUrl.cpp
  xbmc/DynamicDll.cpp
  xbmc/FileItem.cpp
  xbmc/FileItemListModification.cpp
  xbmc/GUIInfoManager.cpp
  xbmc/GUILargeTextureManager.cpp
  xbmc/GUIPassword.cpp
  xbmc/InfoScanner.cpp
  xbmc/LangInfo.cpp
  xbmc/MediaSource.cpp
  xbmc/NfoFile.cpp
  xbmc/PartyModeManager.cpp
  xbmc/PasswordManager.cpp
  xbmc/PlayListPlayer.cpp
  xbmc/SectionLoader.cpp
  xbmc/SeekHandler.cpp
  xbmc/ServiceBroker.cpp
  xbmc/ServiceManager.cpp
  xbmc/SystemGlobals.cpp
  xbmc/TextureCache.cpp
  xbmc/TextureCacheJob.cpp
  xbmc/TextureDatabase.cpp
  xbmc/ThumbLoader.cpp
  xbmc/unimplemented.cpp
  xbmc/URL.cpp
  xbmc/Util.cpp
  xbmc/XBDateTime.cpp
  xbmc/addons/Addon.cpp
  xbmc/addons/AddonBuilder.cpp
  xbmc/addons/AddonDatabase.cpp
  xbmc/addons/AddonInstaller.cpp
  xbmc/addons/AddonManager.cpp
  xbmc/addons/AddonRepos.cpp
  xbmc/addons/AddonStatusHandler.cpp
  xbmc/addons/AddonSystemSettings.cpp
  xbmc/addons/AddonUpdateRules.cpp
  xbmc/addons/AddonVersion.cpp
  xbmc/addons/BinaryAddonCache.cpp
  xbmc/addons/ContextMenuAddon.cpp
  xbmc/addons/ContextMenus.cpp
  xbmc/addons/ExtsMimeSupportList.cpp
  xbmc/addons/FilesystemInstaller.cpp
  xbmc/addons/FontResource.cpp
  xbmc/addons/GameResource.cpp
  xbmc/addons/ImageResource.cpp
  xbmc/addons/LanguageResource.cpp
  xbmc/addons/PluginSource.cpp
  xbmc/addons/Repository.cpp
  xbmc/addons/RepositoryUpdater.cpp
  xbmc/addons/Scraper.cpp
  xbmc/addons/Service.cpp
  xbmc/addons/Skin.cpp
  xbmc/addons/UISoundsResource.cpp
  xbmc/addons/Webinterface.cpp
  xbmc/addons/addoninfo/AddonExtensions.cpp
  xbmc/addons/addoninfo/AddonInfo.cpp
  xbmc/addons/addoninfo/AddonInfoBuilder.cpp
  xbmc/addons/addoninfo/AddonType.cpp
  xbmc/addons/gui/GUIDialogAddonInfo.cpp
  xbmc/addons/gui/GUIDialogAddonSettings.cpp
  xbmc/addons/gui/GUIHelpers.cpp
  xbmc/addons/gui/GUIViewStateAddonBrowser.cpp
  xbmc/addons/gui/GUIWindowAddonBrowser.cpp
  xbmc/addons/gui/skin/SkinTimer.cpp
  xbmc/addons/gui/skin/SkinTimerManager.cpp
  xbmc/addons/settings/AddonSettings.cpp
  xbmc/addons/settings/SettingUrlEncodedString.cpp
  xbmc/application/AppParams.cpp
  xbmc/application/AppEnvironment.cpp
  xbmc/application/Application.cpp
  xbmc/application/ApplicationActionListeners.cpp
  xbmc/application/ApplicationPlayer.cpp
  xbmc/application/ApplicationPlayerCallback.cpp
  xbmc/application/ApplicationPowerHandling.cpp
  xbmc/application/ApplicationSettingsHandling.cpp
  xbmc/application/ApplicationSkinHandling.cpp
  xbmc/application/ApplicationStackHelper.cpp
  xbmc/application/AppParamParser.cpp
  xbmc/commons/Exception.cpp
  xbmc/commons/ilog.cpp
  xbmc/cores/DllLoader/coff.cpp
  xbmc/cores/DllLoader/dll.cpp
  xbmc/cores/DllLoader/dll_tracker.cpp
  xbmc/cores/DllLoader/dll_tracker_critical_section.cpp
  xbmc/cores/DllLoader/dll_tracker_file.cpp
  xbmc/cores/DllLoader/dll_tracker_library.cpp
  xbmc/cores/DllLoader/dll_tracker_memory.cpp
  # xbmc/cores/DllLoader/dll_tracker_socket.cpp
  xbmc/cores/DllLoader/dll_util.cpp
  xbmc/cores/DllLoader/DllLoader.cpp
  xbmc/cores/DllLoader/DllLoaderContainer.cpp
  xbmc/cores/DllLoader/LibraryLoader.cpp
  xbmc/cores/DllLoader/exports/emu_dummy.cpp
  # xbmc/cores/DllLoader/exports/emu_dx8.cpp
  xbmc/cores/DllLoader/exports/emu_kernel32.cpp
  xbmc/cores/DllLoader/exports/emu_misc.cpp
  xbmc/cores/DllLoader/exports/emu_msvcrt.cpp
  xbmc/cores/DllLoader/exports/emu_ole32.cpp
  xbmc/cores/DllLoader/exports/emu_registry.cpp
  # xbmc/cores/DllLoader/exports/emu_socket.cpp
  xbmc/cores/DllLoader/exports/exports_kernel32.cpp
  xbmc/cores/DllLoader/exports/exports_misc.cpp
  xbmc/cores/DllLoader/exports/exports_msvcrt.cpp
  xbmc/cores/DllLoader/exports/exports_ole32.cpp
  xbmc/cores/DllLoader/exports/exports_python.cpp
  xbmc/cores/DllLoader/exports/exports_reg.cpp
  xbmc/cores/DllLoader/exports/exports_socket.cpp
  xbmc/cores/DllLoader/exports/exports_xbox_dx8.cpp
  xbmc/cores/DllLoader/exports/nxdk_stubs.cpp
  # xbmc/cores/DllLoader/exports/emu_socket/getaddrinfo.c
  # xbmc/cores/DllLoader/exports/emu_socket/getnameinfo.c
  # xbmc/cores/DllLoader/exports/emu_socket/inet_aton.c
  # xbmc/cores/DllLoader/exports/emu_socket/inet_ntop.c
  # xbmc/cores/DllLoader/exports/emu_socket/inet_pton.c
  xbmc/cores/DllLoader/exports/util/EmuFileWrapper.cpp
  xbmc/cores/DataCacheCore.cpp
  xbmc/cores/VideoSettings.cpp
  xbmc/cores/playercorefactory/PlayerCoreConfig.cpp
  xbmc/cores/playercorefactory/PlayerCoreFactory.cpp
  xbmc/cores/playercorefactory/PlayerSelectionRule.cpp
  xbmc/dbwrappers/Database.cpp
  xbmc/dbwrappers/DatabaseQuery.cpp
  xbmc/dbwrappers/dataset.cpp
  xbmc/dbwrappers/qry_dat.cpp
  xbmc/dbwrappers/sqlitedataset.cpp
  xbmc/dialogs/GUIDialogBoxBase.cpp
  xbmc/dialogs/GUIDialogBusy.cpp
  xbmc/dialogs/GUIDialogButtonMenu.cpp
  xbmc/dialogs/GUIDialogContextMenu.cpp
  xbmc/dialogs/GUIDialogExtendedProgressBar.cpp
  xbmc/dialogs/GUIDialogFileBrowser.cpp
  xbmc/dialogs/GUIDialogGamepad.cpp
  xbmc/dialogs/GUIDialogKaiToast.cpp
  xbmc/dialogs/GUIDialogKeyboardGeneric.cpp
  xbmc/dialogs/GUIDialogMediaFilter.cpp
  xbmc/dialogs/GUIDialogMediaSource.cpp
  xbmc/dialogs/GUIDialogNumeric.cpp
  xbmc/dialogs/GUIDialogOK.cpp
  xbmc/dialogs/GUIDialogPlayEject.cpp
  xbmc/dialogs/GUIDialogProgress.cpp
  xbmc/dialogs/GUIDialogSeekBar.cpp
  xbmc/dialogs/GUIDialogSelect.cpp
  xbmc/dialogs/GUIDialogSlider.cpp
  xbmc/dialogs/GUIDialogSmartPlaylistEditor.cpp
  xbmc/dialogs/GUIDialogSmartPlaylistRule.cpp
  xbmc/dialogs/GUIDialogSubMenu.cpp
  xbmc/dialogs/GUIDialogTextViewer.cpp
  xbmc/dialogs/GUIDialogVolumeBar.cpp
  xbmc/dialogs/GUIDialogYesNo.cpp
  xbmc/favourites/ContextMenus.cpp
  xbmc/favourites/FavouritesService.cpp
  xbmc/favourites/FavouritesURL.cpp
  xbmc/favourites/FavouritesUtils.cpp
  xbmc/favourites/GUIDialogFavourites.cpp
  xbmc/favourites/GUIViewStateFavourites.cpp
  xbmc/favourites/GUIWindowFavourites.cpp
  xbmc/filesystem/AddonsDirectory.cpp
  xbmc/filesystem/CacheStrategy.cpp
  xbmc/filesystem/CircularCache.cpp
  xbmc/filesystem/CurlFile.cpp
  xbmc/filesystem/Directory.cpp
  xbmc/filesystem/DirectoryCache.cpp
  xbmc/filesystem/DirectoryFactory.cpp
  xbmc/filesystem/DirectoryHistory.cpp
  xbmc/filesystem/FavouritesDirectory.cpp
  xbmc/filesystem/File.cpp
  xbmc/filesystem/FileCache.cpp
  xbmc/filesystem/FileDirectoryFactory.cpp
  xbmc/filesystem/FileFactory.cpp
  xbmc/filesystem/IDirectory.cpp
  xbmc/filesystem/IFile.cpp
  xbmc/filesystem/ImageFile.cpp
  xbmc/filesystem/LibraryDirectory.cpp
  xbmc/filesystem/MultiPathDirectory.cpp
  xbmc/filesystem/MultiPathFile.cpp
  xbmc/filesystem/MusicDatabaseDirectory.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNode.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbum.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAdded.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAddedSong.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayed.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayedSong.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100Song.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeArtist.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeDiscs.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeGrouped.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeOverview.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeRoot.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSingles.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSong.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSongTop100.cpp
  xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeTop100.cpp
  xbmc/filesystem/MusicDatabaseDirectory/QueryParams.cpp
  xbmc/filesystem/MusicDatabaseFile.cpp
  xbmc/filesystem/MusicFileDirectory.cpp
  xbmc/filesystem/MusicSearchDirectory.cpp
  xbmc/filesystem/OverrideDirectory.cpp
  xbmc/filesystem/OverrideFile.cpp
  xbmc/filesystem/PlaylistDirectory.cpp
  xbmc/filesystem/PlaylistFileDirectory.cpp
  xbmc/filesystem/PluginDirectory.cpp
  xbmc/filesystem/PluginFile.cpp
  xbmc/filesystem/ResourceDirectory.cpp
  xbmc/filesystem/ResourceFile.cpp
  xbmc/filesystem/SmartPlaylistDirectory.cpp
  xbmc/filesystem/SourcesDirectory.cpp
  xbmc/filesystem/SpecialProtocol.cpp
  xbmc/filesystem/SpecialProtocolDirectory.cpp
  xbmc/filesystem/SpecialProtocolFile.cpp
  xbmc/filesystem/StackDirectory.cpp
  xbmc/filesystem/VideoDatabaseDirectory.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNode.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeEpisodes.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeGrouped.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeInProgressTvShows.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMoviesOverview.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMusicVideosOverview.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeOverview.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedEpisodes.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMovies.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMusicVideos.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRoot.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeSeasons.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMovies.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMusicVideos.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleTvShows.cpp
  xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTvShowsOverview.cpp
  xbmc/filesystem/VideoDatabaseDirectory/QueryParams.cpp
  xbmc/filesystem/VideoDatabaseFile.cpp
  xbmc/filesystem/VirtualDirectory.cpp
  xbmc/filesystem/ZipDirectory.cpp
  xbmc/filesystem/ZipFile.cpp
  xbmc/guilib/cximage.cpp
  xbmc/guilib/DirtyRegionSolvers.cpp
  xbmc/guilib/DirtyRegionTracker.cpp
  xbmc/guilib/GUIAction.cpp
  xbmc/guilib/GUIAudioManager.cpp
  xbmc/guilib/GUIBaseContainer.cpp
  xbmc/guilib/GUIBorderedImage.cpp
  xbmc/guilib/GUIButtonControl.cpp
  xbmc/guilib/GUIColorManager.cpp
  xbmc/guilib/GUIComponent.cpp
  xbmc/guilib/GUIControl.cpp
  xbmc/guilib/GUIControlFactory.cpp
  xbmc/guilib/GUIControlGroup.cpp
  xbmc/guilib/GUIControlGroupList.cpp
  xbmc/guilib/GUIDialog.cpp
  xbmc/guilib/GUIEditControl.cpp
  xbmc/guilib/GUIFadeLabelControl.cpp
  xbmc/guilib/GUIFixedListContainer.cpp
  xbmc/guilib/GUIFont.cpp
  xbmc/guilib/GUIFontCache.cpp
  xbmc/guilib/GUIFontManager.cpp
  xbmc/guilib/GUIFontTTF.cpp
  xbmc/guilib/GUIFontTTFGL.cpp
  xbmc/guilib/GUIImage.cpp
  xbmc/guilib/GUIIncludes.cpp
  xbmc/guilib/GUIKeyboardFactory.cpp
  xbmc/guilib/GUILabel.cpp
  xbmc/guilib/GUILabelControl.cpp
  xbmc/guilib/GUIListContainer.cpp
  xbmc/guilib/GUIListGroup.cpp
  xbmc/guilib/GUIListItem.cpp
  xbmc/guilib/GUIListItemLayout.cpp
  xbmc/guilib/GUIListLabel.cpp
  xbmc/guilib/GUIMessage.cpp
  xbmc/guilib/GUIMoverControl.cpp
  xbmc/guilib/GUIMultiImage.cpp
  xbmc/guilib/GUIPanelContainer.cpp
  xbmc/guilib/GUIProgressControl.cpp
  xbmc/guilib/GUIRadioButtonControl.cpp
  xbmc/guilib/GUIRenderingControl.cpp
  xbmc/guilib/GUIResizeControl.cpp
  xbmc/guilib/GUIScrollBarControl.cpp
  xbmc/guilib/GUISettingsSliderControl.cpp
  xbmc/guilib/GUISliderControl.cpp
  xbmc/guilib/GUISound.cpp
  xbmc/guilib/GUISpinControl.cpp
  xbmc/guilib/GUISpinControlEx.cpp
  xbmc/guilib/GUIStaticItem.cpp
  xbmc/guilib/GUITextBox.cpp
  xbmc/guilib/GUITextLayout.cpp
  xbmc/guilib/GUITexture.cpp
  xbmc/guilib/GUITextureGL.cpp
  xbmc/guilib/GUIToggleButtonControl.cpp
  xbmc/guilib/GUIVideoControl.cpp
  xbmc/guilib/GUIWindow.cpp
  xbmc/guilib/GUIWindowManager.cpp
  xbmc/guilib/GUIWrappingListContainer.cpp
  xbmc/guilib/GraphicContext.cpp
  xbmc/guilib/IWindowManagerCallback.cpp
  xbmc/guilib/JpegIO.cpp
  xbmc/guilib/LocalizeStrings.cpp
  xbmc/guilib/MatrixGLES.cpp
  xbmc/guilib/Resolution.cpp
  xbmc/guilib/Texture.cpp
  xbmc/guilib/TextureGL.cpp
  xbmc/guilib/TextureManager.cpp
  xbmc/guilib/VisibleEffect.cpp
  xbmc/guilib/guiinfo/AddonsGUIInfo.cpp
  xbmc/guilib/guiinfo/GUIControlsGUIInfo.cpp
  xbmc/guilib/guiinfo/GUIInfo.cpp
  xbmc/guilib/guiinfo/GUIInfoBool.cpp
  xbmc/guilib/guiinfo/GUIInfoColor.cpp
  xbmc/guilib/guiinfo/GUIInfoHelper.cpp
  xbmc/guilib/guiinfo/GUIInfoLabel.cpp
  xbmc/guilib/guiinfo/GUIInfoProviders.cpp
  xbmc/guilib/guiinfo/LibraryGUIInfo.cpp
  xbmc/guilib/guiinfo/MusicGUIInfo.cpp
  xbmc/guilib/guiinfo/PicturesGUIInfo.cpp
  xbmc/guilib/guiinfo/PlayerGUIInfo.cpp
  xbmc/guilib/guiinfo/SkinGUIInfo.cpp
  xbmc/guilib/guiinfo/SystemGUIInfo.cpp
  xbmc/guilib/guiinfo/VideoGUIInfo.cpp
  xbmc/guilib/guiinfo/VisualisationGUIInfo.cpp
  xbmc/guilib/guiinfo/WeatherGUIInfo.cpp
  xbmc/guilib/imagefactory.cpp
  xbmc/input/AppTranslator.cpp
  xbmc/input/ButtonTranslator.cpp
  xbmc/input/GamepadTranslator.cpp
  xbmc/input/InputCodingTableBasePY.cpp
  xbmc/input/InputCodingTableFactory.cpp
  xbmc/input/InputCodingTableKorean.cpp
  xbmc/input/InputManager.cpp
  xbmc/input/Key.cpp
  xbmc/input/KeyboardLayout.cpp
  xbmc/input/KeyboardLayoutManager.cpp
  xbmc/input/WindowTranslator.cpp
  xbmc/input/XBMC_keytable.cpp
  xbmc/input/actions/Action.cpp
  xbmc/input/actions/ActionTranslator.cpp
  xbmc/interfaces/AnnouncementManager.cpp
  xbmc/interfaces/builtins/AddonBuiltins.cpp
  xbmc/interfaces/builtins/ApplicationBuiltins.cpp
  xbmc/interfaces/builtins/Builtins.cpp
  xbmc/interfaces/builtins/CECBuiltins.cpp
  xbmc/interfaces/builtins/GUIBuiltins.cpp
  xbmc/interfaces/builtins/GUIContainerBuiltins.cpp
  xbmc/interfaces/builtins/GUIControlBuiltins.cpp
  xbmc/interfaces/builtins/LibraryBuiltins.cpp
  xbmc/interfaces/builtins/OpticalBuiltins.cpp
  xbmc/interfaces/builtins/PictureBuiltins.cpp
  xbmc/interfaces/builtins/PlayerBuiltins.cpp
  xbmc/interfaces/builtins/ProfileBuiltins.cpp
  xbmc/interfaces/builtins/SkinBuiltins.cpp
  xbmc/interfaces/builtins/SystemBuiltins.cpp
  xbmc/interfaces/builtins/WeatherBuiltins.cpp
  xbmc/interfaces/generic/ILanguageInvoker.cpp
  xbmc/interfaces/generic/LanguageInvokerThread.cpp
  xbmc/interfaces/generic/RunningScriptObserver.cpp
  xbmc/interfaces/generic/ScriptInvocationManager.cpp
  xbmc/interfaces/generic/ScriptRunner.cpp
  xbmc/interfaces/info/InfoBool.cpp
  xbmc/interfaces/info/InfoExpression.cpp
  xbmc/interfaces/info/SkinVariable.cpp
  xbmc/listproviders/DirectoryProvider.cpp
  xbmc/listproviders/IListProvider.cpp
  xbmc/listproviders/MultiProvider.cpp
  xbmc/listproviders/StaticProvider.cpp
  xbmc/media/MediaType.cpp
  xbmc/messaging/ApplicationMessenger.cpp
  xbmc/messaging/helpers/DialogHelper.cpp
  xbmc/messaging/helpers/DialogOKHelper.cpp
  xbmc/music/Album.cpp
  xbmc/music/Artist.cpp
  xbmc/music/ContextMenus.cpp
  xbmc/music/GUIViewStateMusic.cpp
  xbmc/music/MusicDatabase.cpp
  xbmc/music/MusicDbUrl.cpp
  xbmc/music/MusicInfoLoader.cpp
  xbmc/music/MusicLibraryQueue.cpp
  xbmc/music/MusicThumbLoader.cpp
  xbmc/music/MusicUtils.cpp
  xbmc/music/Song.cpp
  xbmc/music/dialogs/GUIDialogInfoProviderSettings.cpp
  xbmc/music/dialogs/GUIDialogMusicInfo.cpp
  xbmc/music/dialogs/GUIDialogMusicOSD.cpp
  xbmc/music/dialogs/GUIDialogSongInfo.cpp
  xbmc/music/dialogs/GUIDialogVisualisationPresetList.cpp
  xbmc/music/infoscanner/MusicAlbumInfo.cpp
  xbmc/music/infoscanner/MusicArtistInfo.cpp
  xbmc/music/infoscanner/MusicInfoScanner.cpp
  xbmc/music/infoscanner/MusicInfoScraper.cpp
  xbmc/music/jobs/MusicLibraryCleaningJob.cpp
  xbmc/music/jobs/MusicLibraryExportJob.cpp
  xbmc/music/jobs/MusicLibraryImportJob.cpp
  xbmc/music/jobs/MusicLibraryJob.cpp
  xbmc/music/jobs/MusicLibraryProgressJob.cpp
  xbmc/music/jobs/MusicLibraryScanningJob.cpp
  xbmc/music/tags/MusicInfoTag.cpp
  xbmc/music/tags/MusicInfoTagLoaderCDDA.cpp
  xbmc/music/tags/MusicInfoTagLoaderDatabase.cpp
  xbmc/music/tags/MusicInfoTagLoaderFFmpeg.cpp
  xbmc/music/tags/MusicInfoTagLoaderFactory.cpp
  xbmc/music/tags/MusicInfoTagLoaderShn.cpp
  xbmc/music/tags/ReplayGain.cpp
  xbmc/music/tags/TagLoaderTagLib.cpp
  xbmc/music/windows/GUIWindowMusicBase.cpp
  xbmc/music/windows/GUIWindowMusicNav.cpp
  xbmc/music/windows/GUIWindowMusicPlaylist.cpp
  xbmc/music/windows/GUIWindowMusicPlaylistEditor.cpp
  xbmc/music/windows/GUIWindowVisualisation.cpp
  xbmc/music/windows/MusicFileItemListModifier.cpp
  xbmc/network/DNSNameCache.cpp
  xbmc/pictures/ExifParse.cpp
  xbmc/pictures/GUIDialogPictureInfo.cpp
  xbmc/pictures/GUIViewStatePictures.cpp
  xbmc/pictures/GUIWindowPictures.cpp
  xbmc/pictures/GUIWindowSlideShow.cpp
  xbmc/pictures/IptcParse.cpp
  xbmc/pictures/JpegParse.cpp
  xbmc/pictures/Picture.cpp
  xbmc/pictures/PictureInfoLoader.cpp
  xbmc/pictures/PictureInfoTag.cpp
  xbmc/pictures/PictureThumbLoader.cpp
  xbmc/pictures/SlideShowPicture.cpp
  xbmc/pictures/libexif.cpp
  xbmc/platform/win32/CharsetConverter.cpp
  xbmc/platform/win32/Filesystem.cpp
  xbmc/platform/win32/XTimeUtils.cpp
  xbmc/platform/win32/filesystem/Win32Directory.cpp
  xbmc/platform/win32/filesystem/Win32File.cpp
  xbmc/platform/win32/threads/ThreadImplWin.cpp
  xbmc/platform/win32/utils/Win32InterfaceForCLog.cpp
  xbmc/platform/xbmc.cpp
  xbmc/platform/xbox/AudioContext.cpp
  xbmc/platform/xbox/CPUInfoXbox.cpp
  xbmc/platform/xbox/main.cpp
  xbmc/platform/xbox/MessagePrinter.cpp
  xbmc/platform/xbox/XBOXUtil.cpp
  xbmc/platform/xbox/filesystem/XboxDirectory.cpp
  xbmc/platform/xbox/filesystem/XboxFile.cpp
  xbmc/platform/xbox/storage/XboxStorageProvider.cpp
  xbmc/platform/xbox/strptime.cpp
  xbmc/playlists/PlayList.cpp
  xbmc/playlists/PlayListB4S.cpp
  xbmc/playlists/PlayListFactory.cpp
  xbmc/playlists/PlayListM3U.cpp
  xbmc/playlists/PlayListPLS.cpp
  xbmc/playlists/PlayListURL.cpp
  xbmc/playlists/PlayListWPL.cpp
  xbmc/playlists/PlayListXML.cpp
  xbmc/playlists/PlayListXSPF.cpp
  xbmc/playlists/SmartPlayList.cpp
  xbmc/playlists/SmartPlaylistFileItemListModifier.cpp
  xbmc/profiles/Profile.cpp
  xbmc/profiles/ProfileManager.cpp
  xbmc/profiles/dialogs/GUIDialogLockSettings.cpp
  xbmc/profiles/dialogs/GUIDialogProfileSettings.cpp
  xbmc/profiles/windows/GUIWindowSettingsProfile.cpp
  xbmc/programs/GUIViewStatePrograms.cpp
  xbmc/programs/GUIWindowPrograms.cpp
  xbmc/rendering/RenderSystem.cpp
  xbmc/rendering/gl/RenderSystemGL.cpp
  xbmc/settings/AdvancedSettings.cpp
  xbmc/settings/DisplaySettings.cpp
  xbmc/settings/LibExportSettings.cpp
  xbmc/settings/MediaSettings.cpp
  xbmc/settings/MediaSourceSettings.cpp
  xbmc/settings/SettingAddon.cpp
  xbmc/settings/SettingConditions.cpp
  xbmc/settings/SettingControl.cpp
  xbmc/settings/SettingCreator.cpp
  xbmc/settings/SettingDateTime.cpp
  xbmc/settings/SettingPath.cpp
  xbmc/settings/SettingUtils.cpp
  xbmc/settings/Settings.cpp
  xbmc/settings/SettingsBase.cpp
  xbmc/settings/SettingsComponent.cpp
  xbmc/settings/SettingsValueFlatJsonSerializer.cpp
  xbmc/settings/SettingsValueXmlSerializer.cpp
  xbmc/settings/SkinSettings.cpp
  xbmc/settings/dialogs/GUIDialogContentSettings.cpp
  xbmc/settings/dialogs/GUIDialogLibExportSettings.cpp
  xbmc/settings/dialogs/GUIDialogSettingsBase.cpp
  xbmc/settings/dialogs/GUIDialogSettingsManagerBase.cpp
  xbmc/settings/dialogs/GUIDialogSettingsManualBase.cpp
  xbmc/settings/lib/ISetting.cpp
  xbmc/settings/lib/ISettingControl.cpp
  xbmc/settings/lib/Setting.cpp
  xbmc/settings/lib/SettingCategoryAccess.cpp
  xbmc/settings/lib/SettingConditions.cpp
  xbmc/settings/lib/SettingDependency.cpp
  xbmc/settings/lib/SettingRequirement.cpp
  xbmc/settings/lib/SettingSection.cpp
  xbmc/settings/lib/SettingUpdate.cpp
  xbmc/settings/lib/SettingsManager.cpp
  xbmc/settings/windows/GUIControlSettings.cpp
  xbmc/settings/windows/GUIWindowSettings.cpp
  xbmc/settings/windows/GUIWindowSettingsCategory.cpp
  xbmc/storage/MediaManager.cpp
  xbmc/threads/Event.cpp
  xbmc/threads/Thread.cpp
  xbmc/threads/Timer.cpp
  xbmc/utils/AlarmClock.cpp
  xbmc/utils/Archive.cpp
  xbmc/utils/Base64.cpp
  xbmc/utils/BitstreamStats.cpp
  xbmc/utils/BooleanLogic.cpp
  xbmc/utils/CPUInfo.cpp
  xbmc/utils/CharsetConverter.cpp
  xbmc/utils/CharsetDetection.cpp
  xbmc/utils/ColorUtils.cpp
  xbmc/utils/ContentUtils.cpp
  xbmc/utils/Crc32.cpp
  xbmc/utils/DatabaseUtils.cpp
  xbmc/utils/Digest.cpp
  xbmc/utils/DiscsUtils.cpp
  xbmc/utils/EmbeddedArt.cpp
  xbmc/utils/ExecString.cpp
  xbmc/utils/Fanart.cpp
  xbmc/utils/FileExtensionProvider.cpp
  xbmc/utils/FileOperationJob.cpp
  xbmc/utils/FileUtils.cpp
  xbmc/utils/GLUtils.cpp
  xbmc/utils/GroupUtils.cpp
  xbmc/utils/HTMLUtil.cpp
  xbmc/utils/HttpHeader.cpp
  xbmc/utils/HttpParser.cpp
  xbmc/utils/HttpRangeUtils.cpp
  xbmc/utils/HttpResponse.cpp
  xbmc/utils/InfoLoader.cpp
  xbmc/utils/JSONVariantParser.cpp
  xbmc/utils/JSONVariantWriter.cpp
  xbmc/utils/JobManager.cpp
  xbmc/utils/LabelFormatter.cpp
  xbmc/utils/LangCodeExpander.cpp
  xbmc/utils/LegacyPathTranslation.cpp
  xbmc/utils/Locale.cpp
  xbmc/utils/Mime.cpp
  xbmc/utils/MovingSpeed.cpp
  xbmc/utils/Observer.cpp
  xbmc/utils/POUtils.cpp
  xbmc/utils/PlayerUtils.cpp
  xbmc/utils/ProgressJob.cpp
  xbmc/utils/RecentlyAddedJob.cpp
  xbmc/utils/RegExp.cpp
  xbmc/utils/RingBuffer.cpp
  xbmc/utils/SaveFileStateJob.cpp
  xbmc/utils/ScraperParser.cpp
  xbmc/utils/ScraperUrl.cpp
  xbmc/utils/SortUtils.cpp
  xbmc/utils/Speed.cpp
  xbmc/utils/Splash.cpp
  xbmc/utils/StreamDetails.cpp
  xbmc/utils/StreamUtils.cpp
  xbmc/utils/StringUtils.cpp
  xbmc/utils/StringValidation.cpp
  xbmc/utils/SystemInfo.cpp
  xbmc/utils/Temperature.cpp
  xbmc/utils/TimeUtils.cpp
  xbmc/utils/URIUtils.cpp
  xbmc/utils/UrlOptions.cpp
  xbmc/utils/Utf8Utils.cpp
  xbmc/utils/Variant.cpp
  xbmc/utils/XBMCTinyXML.cpp
  xbmc/utils/XBMCTinyXML2.cpp
  xbmc/utils/XMLUtils.cpp
  xbmc/utils/auto_buffer.cpp
  xbmc/utils/log.cpp
  xbmc/utils/md5.cpp
  xbmc/video/Bookmark.cpp
  xbmc/video/ContextMenus.cpp
  xbmc/video/GUIViewStateVideo.cpp
  xbmc/video/PlayerController.cpp
  xbmc/video/VideoDatabase.cpp
  xbmc/video/VideoDbUrl.cpp
  xbmc/video/VideoInfoDownloader.cpp
  xbmc/video/VideoInfoScanner.cpp
  xbmc/video/VideoInfoTag.cpp
  xbmc/video/VideoLibraryQueue.cpp
  xbmc/video/VideoThumbLoader.cpp
  xbmc/video/VideoUtils.cpp
  xbmc/video/ViewModeSettings.cpp
  xbmc/video/dialogs/GUIDialogAudioSettings.cpp
  xbmc/video/dialogs/GUIDialogFullScreenInfo.cpp
  xbmc/video/dialogs/GUIDialogSubtitleSettings.cpp
  xbmc/video/dialogs/GUIDialogSubtitles.cpp
  xbmc/video/dialogs/GUIDialogVideoBookmarks.cpp
  xbmc/video/dialogs/GUIDialogVideoInfo.cpp
  xbmc/video/dialogs/GUIDialogVideoOSD.cpp
  xbmc/video/dialogs/GUIDialogVideoSettings.cpp
  xbmc/video/jobs/VideoLibraryCleaningJob.cpp
  xbmc/video/jobs/VideoLibraryJob.cpp
  xbmc/video/jobs/VideoLibraryMarkWatchedJob.cpp
  xbmc/video/jobs/VideoLibraryProgressJob.cpp
  xbmc/video/jobs/VideoLibraryRefreshingJob.cpp
  xbmc/video/jobs/VideoLibraryResetResumePointJob.cpp
  xbmc/video/jobs/VideoLibraryScanningJob.cpp
  xbmc/video/tags/VideoInfoTagLoaderFactory.cpp
  xbmc/video/tags/VideoTagLoaderNFO.cpp
  xbmc/video/tags/VideoTagLoaderPlugin.cpp
  xbmc/video/windows/GUIWindowVideoBase.cpp
  xbmc/video/windows/GUIWindowVideoNav.cpp
  xbmc/video/windows/GUIWindowVideoPlaylist.cpp
  xbmc/video/windows/VideoFileItemListModifier.cpp
  xbmc/view/GUIViewControl.cpp
  xbmc/view/GUIViewState.cpp
  xbmc/view/ViewDatabase.cpp
  xbmc/view/ViewStateSettings.cpp
  xbmc/weather/GUIWindowWeather.cpp
  xbmc/weather/WeatherJob.cpp
  xbmc/weather/WeatherManager.cpp
  xbmc/windowing/WinSystem.cpp
  xbmc/windowing/xbox/WinSystemXboxGL.cpp
  xbmc/windows/GUIMediaWindow.cpp
  xbmc/windows/GUIWindowFileManager.cpp
  xbmc/windows/GUIWindowHome.cpp
  xbmc/windows/GUIWindowLoginScreen.cpp
  xbmc/windows/GUIWindowSplash.cpp
  xbmc/windows/GUIWindowStartup.cpp
  xbmc/windows/GUIWindowSystemInfo.cpp
)

target_include_directories(xbmc PRIVATE xbmc xbmc/cores/VideoPlayer xbmc/platform/xbox)

# Bring in the DVD drive automount support
target_link_libraries(xbmc PUBLIC ${NXDK_DIR}/lib/libnxdk_automount_d.lib)
target_link_options(xbmc PRIVATE "-include:_automount_d_drive")

# Bring in networking support
target_link_libraries(xbmc PUBLIC ${NXDK_DIR}/lib/libnxdk_net.lib)
target_include_directories(xbmc PRIVATE ${NXDK_DIR}/lib/net/lwip/src/include ${NXDK_DIR}/lib/net/nforceif/include ${NXDK_DIR}/lib/net/nvnetdrv)

# Bring in pbkit support
target_link_libraries(xbmc PUBLIC ${NXDK_DIR}/lib/libpbkit.lib)

# Bring in libjpeg-turbo support
target_link_libraries(xbmc PUBLIC ${NXDK_DIR}/lib/libjpeg.lib)
target_include_directories(xbmc PRIVATE ${NXDK_DIR}/lib/libjpeg ${NXDK_DIR}/lib/libjpeg/libjpeg-turbo)

# Bring in zlib support
target_link_libraries(xbmc PUBLIC ${NXDK_DIR}/lib/libzlib.lib)
target_include_directories(xbmc PRIVATE ${NXDK_DIR}/lib/zlib/zlib)

# Bring in fribidi library
add_subdirectory(lib/fribidi)
target_link_libraries(xbmc PUBLIC fribidi)

# Bring in fstrcmp library
add_subdirectory(lib/fstrcmp)
target_link_libraries(xbmc PUBLIC fstrcmp)

# Bring in libiconv library
add_subdirectory(lib/libiconv)
target_link_libraries(xbmc PUBLIC libiconv)

# Bring in SQLite library
add_subdirectory(lib/sqlite)
target_link_libraries(xbmc PUBLIC sqlite)

# Bring in FMT library
message(STATUS "Downloading fmtlib")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        12.1.0
  GIT_PROGRESS TRUE
  PATCH_COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/lib/fmt_nxdk.patch
)
set(FMT_OS OFF CACHE BOOL "Disale OS module")
add_definitions(
  -DFMT_USE_WRITE_CONSOLE
)
FetchContent_MakeAvailable(fmt)
target_link_libraries(xbmc PRIVATE fmt::fmt)

# Bring in JSON library
message(STATUS "Downloading nlohmann_json")
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.12.0
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(json)
target_link_libraries(xbmc PRIVATE nlohmann_json::nlohmann_json)

# Bring in TinyXML 2.6.2
message(STATUS "Downloading tinyxml")
FetchContent_Declare(
  tinyxml
  GIT_REPOSITORY https://github.com/xbmc4xbox/tinyxml.git
  GIT_TAG        b3c39f382075b73cbec7e88687b5762ba11ef65c
  GIT_PROGRESS TRUE
  PATCH_COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/lib/tinyxml_nxdk.patch
)
FetchContent_MakeAvailable(tinyxml)
target_link_libraries(xbmc PRIVATE tinyxml)

# Bring in TinyXML2
message(STATUS "Downloading tinyxml2")
FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG        11.0.0
  GIT_PROGRESS TRUE
  PATCH_COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/lib/tinyxml2_nxdk.patch
)
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "Disable tinyxml2 tests")
FetchContent_MakeAvailable(tinyxml2)
target_link_libraries(xbmc PRIVATE tinyxml2::tinyxml2)

# Bring in PCRE2
message(STATUS "Downloading pcre2")
FetchContent_Declare(
  pcre2
  GIT_REPOSITORY https://github.com/PCRE2Project/pcre2.git
  GIT_TAG        pcre2-10.47
  GIT_PROGRESS TRUE
)
set(PCRE2_BUILD_TESTS OFF CACHE BOOL "Disable PCRE2 tests")
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "Disable building pcre2grep program")
FetchContent_MakeAvailable(pcre2)
target_link_libraries(xbmc PRIVATE pcre2-8)

# Bring in crossguid
message(STATUS "Downloading crossguid")
FetchContent_Declare(
  crossguid
  GIT_REPOSITORY https://github.com/graeme-hill/crossguid.git
  GIT_TAG        ca1bf4b810e2d188d04cb6286f957008ee1b7681
  GIT_PROGRESS TRUE
  PATCH_COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/lib/crossguid_nxdk.patch
)
set(CROSSGUID_TESTS OFF CACHE BOOL "Disablee crossguid tests")
FetchContent_MakeAvailable(crossguid)
target_link_libraries(xbmc PRIVATE crossguid)

# Bring in pbgl
message(STATUS "Downloading pbgl")
FetchContent_Declare(
  pbgl
  GIT_REPOSITORY https://github.com/fgsfdsfgs/pbgl.git
  GIT_TAG        9fbfdd63ec9040e52762bf76daccdb4158abc21f
  GIT_PROGRESS TRUE
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DPBGL_DEBUG)
endif()
FetchContent_MakeAvailable(pbgl)
target_link_libraries(xbmc PRIVATE pbgl)

# Bring in freetype
message(STATUS "Downloading freetype")
FetchContent_Declare(
  freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG        VER-2-14-1
  GIT_PROGRESS TRUE
)
set(FT_DISABLE_ZLIB FALSE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)
target_link_libraries(xbmc PRIVATE freetype)

#Pre-build commands

#Post-build commands
add_xbox_build_steps(xbmc ${XBE_TITLE} ${XBOX_ISO_DIR})
