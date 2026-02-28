/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "MediaSource.h" // Definition of VECSOURCES
#include "utils/Digest.h"

#include <climits>
#include <cmath>
#include <stdint.h>
#include <string.h>
#include <vector>

// A list of filesystem types for LegalPath/FileName
#define LEGAL_NONE            0
#define LEGAL_WIN32_COMPAT    1
#define LEGAL_FATX            2

#define ARRAY_SIZE(X)         (sizeof(X)/sizeof((X)[0]))

class CFileItem;
class CFileItemList;
class CURL;

class CUtil
{
  CUtil() = delete;
public:
  static void CleanString(const std::string& strFileName,
                          std::string& strTitle,
                          std::string& strTitleAndYear,
                          std::string& strYear,
                          bool bRemoveExtension = false,
                          bool bCleanChars = true);
  static std::string GetTitleFromPath(const CURL& url, bool bIsFolder = false);
  static std::string GetTitleFromPath(const std::string& strFileNameAndPath, bool bIsFolder = false);
  static void GetQualifiedFilename(const std::string &strBasePath, std::string &strFilename);
  static bool ExcludeFileOrFolder(const std::string& strFileOrFolder, const std::vector<std::string>& regexps);

  static bool IsPicture(const std::string& strFile);
  /// Get resolved filesystem location of splash image
  static std::string GetSplashPath();

  /*! \brief retrieve MD5sum of a file
   \param strPath - path to the file to MD5sum
   \return md5 sum of the file
   */
  static std::string GetFileDigest(const std::string& strPath, KODI::UTILITY::CDigest::Type type);
  static void GetDVDDriveIcon(const std::string& strPath, std::string& strIcon);
  static void RemoveTempFiles();

  static std::string ValidatePath(const std::string &path, bool bFixDoubleSlashes = false); ///< return a validated path, with correct directory separators.

  static std::string GetNextFilename(const std::string &fn_template, int max);

#if defined(TARGET_WINDOWS) || defined(_XBOX)
  static std::string MakeLegalFileName(const std::string &strFile, int LegalType=LEGAL_WIN32_COMPAT);
  static std::string MakeLegalPath(const std::string &strPath, int LegalType=LEGAL_WIN32_COMPAT);
#else
  static std::string MakeLegalFileName(const std::string &strFile, int LegalType=LEGAL_NONE);
  static std::string MakeLegalPath(const std::string &strPath, int LegalType=LEGAL_NONE);
#endif

  /*! \brief Split a comma separated parameter list into separate parameters.
   Takes care of the case where we may have a quoted string containing commas, or we may
   have a function (i.e. parentheses) with multiple parameters as a single parameter.

   eg:

    foo, bar(param1, param2), foo

   will return:

    "foo", "bar(param1, param2)", and "foo".

   \param paramString the string to break up
   \param parameters the returned parameters
   */
  static void SplitParams(const std::string &paramString, std::vector<std::string> &parameters);
  static int GetMatchingSource(const std::string& strPath, VECSOURCES& VECSOURCES, bool& bIsSourceName);
  static std::string TranslateSpecialSource(const std::string &strSpecial);
  static void DeleteDirectoryCache(const std::string &prefix = "");
  static void DeleteMusicDatabaseDirectoryCache();
  static void DeleteVideoDatabaseDirectoryCache();
  static std::string MusicPlaylistsLocation();
  static std::string VideoPlaylistsLocation();

  static void GetSkinThemes(std::vector<std::string>& vecTheme);
  static void GetRecursiveListing(const std::string& strPath, CFileItemList& items, const std::string& strMask, unsigned int flags = 0 /* DIR_FLAG_DEFAULTS */);
  static void GetRecursiveDirsListing(const std::string& strPath, CFileItemList& items, unsigned int flags = 0 /* DIR_FLAG_DEFAULTS */);
  static void ForceForwardSlashes(std::string& strPath);

  static double AlbumRelevance(const std::string& strAlbumTemp1, const std::string& strAlbum1, const std::string& strArtistTemp1, const std::string& strArtist1);
  static bool MakeShortenPath(std::string StrInput, std::string& StrOutput, size_t iTextMaxLength);
  /*! \brief Checks whether the supplied path supports Write file operations (e.g. Rename, Delete, ...)

   \param strPath the path to be checked

   \return true if Write file operations are supported, false otherwise
   */
  static bool SupportsWriteFileOperations(const std::string& strPath);
  /*! \brief Checks whether the supplied path supports Read file operations (e.g. Copy, ...)

   \param strPath the path to be checked

   \return true if Read file operations are supported, false otherwise
   */
  static bool SupportsReadFileOperations(const std::string& strPath);
  static std::string GetDefaultFolderThumb(const std::string &folderThumb);

  static void InitRandomSeed();

  // Get decimal integer representation of roman digit, ivxlcdm are valid
  // return 0 for other chars;
  static int LookupRomanDigit(char roman_digit);
  // Translate a string of roman numerals to decimal a decimal integer
  // return -1 on error, valid range is 1-3999
  static int TranslateRomanNumeral(const char* roman_numeral);

  /*!
   * \brief Thread-safe random number generation
   */
  static int GetRandomNumber();

  static int64_t ConvertSecsToMilliSecs(double secs) { return static_cast<int64_t>(secs * 1000); }
  static double ConvertMilliSecsToSecs(int64_t offset) { return offset / 1000.0; }
  static int64_t ConvertMilliSecsToSecsInt(int64_t offset) { return offset / 1000; }
  static int64_t ConvertMilliSecsToSecsIntRounded(int64_t offset) { return ConvertMilliSecsToSecsInt(offset + 499); }

  /** \brief Copy files from the application bundle over to the user data directory in Application Support/Kodi.
  */
  static void CopyUserDataIfNeeded(const std::string& strPath,
                                   const std::string& file,
                                   const std::string& destname = "");
};


