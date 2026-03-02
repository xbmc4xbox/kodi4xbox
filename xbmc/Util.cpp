/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "ServiceBroker.h"
#include "Util.h"

#include "FileItem.h"
#include "filesystem/Directory.h"
#include "filesystem/MultiPathDirectory.h"
#include "filesystem/SpecialProtocol.h"
#include "filesystem/StackDirectory.h"

#include "profiles/ProfileManager.h"
#include "utils/RegExp.h"
#include "windowing/GraphicContext.h"
#include "guilib/TextureManager.h"
#include "storage/MediaManager.h"
#include "URL.h"
#include "filesystem/File.h"
#include "guilib/LocalizeStrings.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/Digest.h"
#include "utils/FileExtensionProvider.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "utils/log.h"

#include <fstrcmp.h>

using namespace XFILE;
using KODI::UTILITY::CDigest;

#if !defined(TARGET_WINDOWS) && !defined(_XBOX)
unsigned int CUtil::s_randomSeed = time(NULL);
#endif

std::string CUtil::GetTitleFromPath(const std::string& strFileNameAndPath, bool bIsFolder /* = false */)
{
  CURL pathToUrl(strFileNameAndPath);
  return GetTitleFromPath(pathToUrl, bIsFolder);
}

std::string CUtil::GetTitleFromPath(const CURL& url, bool bIsFolder /* = false */)
{
  // use above to get the filename
  std::string path(url.Get());
  URIUtils::RemoveSlashAtEnd(path);
  std::string strFilename = URIUtils::GetFileName(path);

#ifdef HAS_UPNP
  // UPNP
  if (url.IsProtocol("upnp"))
    strFilename = CUPnPDirectory::GetFriendlyName(url);
#endif

  if (url.IsProtocol("rss") || url.IsProtocol("rsss"))
  {
#if 0
    CRSSDirectory dir;
    CFileItemList items;
    if(dir.GetDirectory(url, items) && !items.m_strTitle.empty())
      return items.m_strTitle;
#endif
  }

  // Shoutcast
  else if (url.IsProtocol("shout"))
  {
    const std::string strFileNameAndPath = url.Get();
    const size_t genre = strFileNameAndPath.find_first_of('=');
    if(genre == std::string::npos)
      strFilename = g_localizeStrings.Get(260);
    else
      strFilename = g_localizeStrings.Get(260) + " - " + strFileNameAndPath.substr(genre+1).c_str();
  }

  // Windows SMB Network (SMB)
  else if (url.IsProtocol("smb") && strFilename.empty())
  {
    if (url.GetHostName().empty())
    {
      strFilename = g_localizeStrings.Get(20171);
    }
    else
    {
      strFilename = url.GetHostName();
    }
  }

  // Root file views
  else if (url.IsProtocol("sources"))
    strFilename = g_localizeStrings.Get(744);

  // Music Playlists
  else if (StringUtils::StartsWith(path, "special://musicplaylists"))
    strFilename = g_localizeStrings.Get(136);

  // Video Playlists
  else if (StringUtils::StartsWith(path, "special://videoplaylists"))
    strFilename = g_localizeStrings.Get(136);

  else if (URIUtils::HasParentInHostname(url) && strFilename.empty())
    strFilename = URIUtils::GetFileName(url.GetHostName());

  // now remove the extension if needed
  if (!CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(CSettings::SETTING_FILELISTS_SHOWEXTENSIONS) && !bIsFolder)
  {
    URIUtils::RemoveExtension(strFilename);
    return strFilename;
  }

  // URLDecode since the original path may be an URL
  strFilename = CURL::Decode(strFilename);
  return strFilename;
}

namespace
{
void GetTrailingDiscNumberSegmentInfoFromPath(const std::string& pathIn,
                                              size_t& pos,
                                              std::string& number)
{
  std::string path{pathIn};
  URIUtils::RemoveSlashAtEnd(path);

  pos = std::string::npos;
  number.clear();

  // Handle Disc, Disk and locale specific spellings
  std::string discStr{StringUtils::Format("/{} ", g_localizeStrings.Get(427))};
  size_t discPos = path.rfind(discStr);

  if (discPos == std::string::npos)
  {
    discStr = "/Disc ";
    discPos = path.rfind(discStr);
  }

  if (discPos == std::string::npos)
  {
    discStr = "/Disk ";
    discPos = path.rfind(discStr);
  }

  if (discPos != std::string::npos)
  {
    // Check remainder of path is numeric (eg. Disc 1)
    const std::string discNum{path.substr(discPos + discStr.size())};
    if (discNum.find_first_not_of("0123456789") == std::string::npos)
    {
      pos = discPos;
      number = discNum;
    }
  }
}
} // unnamed namespace

std::string CUtil::RemoveTrailingDiscNumberSegmentFromPath(std::string path)
{
  size_t discPos{std::string::npos};
  std::string discNum;
  GetTrailingDiscNumberSegmentInfoFromPath(path, discPos, discNum);

  if (discPos != std::string::npos)
    path.erase(discPos);

  return path;
}

std::string CUtil::GetDiscNumberFromPath(const std::string& path)
{
  size_t discPos{std::string::npos};
  std::string discNum;
  GetTrailingDiscNumberSegmentInfoFromPath(path, discPos, discNum);
  return discNum;
}

bool CUtil::GetFilenameIdentifier(const std::string& fileName,
                                  std::string& identifierType,
                                  std::string& identifier)
{
  std::string match;
  return GetFilenameIdentifier(fileName, identifierType, identifier, match);
}

bool CUtil::GetFilenameIdentifier(const std::string& fileName,
                                  std::string& identifierType,
                                  std::string& identifier,
                                  std::string& match)
{
  CRegExp reIdentifier(true, CRegExp::autoUtf8);

  const std::shared_ptr<CAdvancedSettings> advancedSettings =
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings();
  if (!reIdentifier.RegComp(advancedSettings->m_videoFilenameIdentifierRegExp))
  {
    CLog::LogF(LOGERROR, "Invalid filename identifier RegExp:'{}'",
               advancedSettings->m_videoFilenameIdentifierRegExp);
    return false;
  }
  else
  {
    if (reIdentifier.RegComp(advancedSettings->m_videoFilenameIdentifierRegExp))
    {
      if (reIdentifier.RegFind(fileName) >= 0)
      {
        match = reIdentifier.GetMatch(0);
        identifierType = reIdentifier.GetMatch(1);
        identifier = reIdentifier.GetMatch(2);
        StringUtils::ToLower(identifierType);
        return true;
      }
    }
  }
  return false;
}

bool CUtil::HasFilenameIdentifier(const std::string& fileName)
{
  std::string identifierType;
  std::string identifier;
  return GetFilenameIdentifier(fileName, identifierType, identifier);
}

void CUtil::CleanString(const std::string& strFileName,
                        std::string& strTitle,
                        std::string& strTitleAndYear,
                        std::string& strYear,
                        bool bRemoveExtension /* = false */,
                        bool bCleanChars /* = true */)
{
  strTitleAndYear = strFileName;

  if (strFileName == "..")
   return;

  std::string identifier;
  std::string identifierType;
  std::string identifierMatch;
  if (GetFilenameIdentifier(strFileName, identifierType, identifier, identifierMatch))
    StringUtils::Replace(strTitleAndYear, identifierMatch, "");

  const std::shared_ptr<CAdvancedSettings> advancedSettings = CServiceBroker::GetSettingsComponent()->GetAdvancedSettings();
  const std::vector<std::string> &regexps = advancedSettings->m_videoCleanStringRegExps;

  CRegExp reTags(true, CRegExp::autoUtf8);
  CRegExp reYear(false, CRegExp::autoUtf8);

  if (!reYear.RegComp(advancedSettings->m_videoCleanDateTimeRegExp))
  {
    CLog::Log(LOGERROR, "{}: Invalid datetime clean RegExp:'{}'", __FUNCTION__,
              advancedSettings->m_videoCleanDateTimeRegExp);
  }
  else
  {
    if (reYear.RegFind(strTitleAndYear.c_str()) >= 0)
    {
      strTitleAndYear = reYear.GetMatch(1);
      strYear = reYear.GetMatch(2);
    }
  }

  URIUtils::RemoveExtension(strTitleAndYear);

  for (const auto &regexp : regexps)
  {
    if (!reTags.RegComp(regexp.c_str()))
    { // invalid regexp - complain in logs
      CLog::Log(LOGERROR, "{}: Invalid string clean RegExp:'{}'", __FUNCTION__, regexp);
      continue;
    }
    int j=0;
    if ((j=reTags.RegFind(strTitleAndYear.c_str())) > 0)
      strTitleAndYear.resize(j);
  }

  // final cleanup - special characters used instead of spaces:
  // all '_' tokens should be replaced by spaces
  // if the file contains no spaces, all '.' tokens should be replaced by
  // spaces - one possibility of a mistake here could be something like:
  // "Dr..StrangeLove" - hopefully no one would have anything like this.
  if (bCleanChars)
  {
    bool initialDots = true;
    bool alreadyContainsSpace = (strTitleAndYear.find(' ') != std::string::npos);

    for (char &c : strTitleAndYear)
    {
      if (c != '.')
        initialDots = false;

      if ((c == '_') || ((!alreadyContainsSpace) && !initialDots && (c == '.')))
      {
        c = ' ';
      }
    }
  }

  StringUtils::Trim(strTitleAndYear);
  strTitle = strTitleAndYear;

  // append year
  if (!strYear.empty())
    strTitleAndYear = strTitle + " (" + strYear + ")";

  // restore extension if needed
  if (!bRemoveExtension)
    strTitleAndYear += URIUtils::GetExtension(strFileName);
}

void CUtil::GetQualifiedFilename(const std::string &strBasePath, std::string &strFilename)
{
  // Check if the filename is a fully qualified URL such as protocol://path/to/file
  CURL plItemUrl(strFilename);
  if (!plItemUrl.GetProtocol().empty())
    return;

  // If the filename starts "x:", "\\" or "/" it's already fully qualified so return
  if (strFilename.size() > 1)
#ifdef TARGET_POSIX
    if ( (strFilename[1] == ':') || (strFilename[0] == '/') )
#else
    if ( strFilename[1] == ':' || (strFilename[0] == '\\' && strFilename[1] == '\\'))
#endif
      return;

  // add to base path and then clean
  strFilename = URIUtils::AddFileToFolder(strBasePath, strFilename);

  // get rid of any /./ or \.\ that happen to be there
  StringUtils::Replace(strFilename, "\\.\\", "\\");
  StringUtils::Replace(strFilename, "/./", "/");

  // now find any "\\..\\" and remove them via GetParentPath
  size_t pos;
  while ((pos = strFilename.find("/../")) != std::string::npos)
  {
    std::string basePath = strFilename.substr(0, pos + 1);
    strFilename.erase(0, pos + 4);
    basePath = URIUtils::GetParentPath(basePath);
    strFilename = URIUtils::AddFileToFolder(basePath, strFilename);
  }
  while ((pos = strFilename.find("\\..\\")) != std::string::npos)
  {
    std::string basePath = strFilename.substr(0, pos + 1);
    strFilename.erase(0, pos + 4);
    basePath = URIUtils::GetParentPath(basePath);
    strFilename = URIUtils::AddFileToFolder(basePath, strFilename);
  }
}

bool CUtil::IsPicture(const std::string& strFile)
{
  return URIUtils::HasExtension(strFile,
                  CServiceBroker::GetFileExtensionProvider().GetPictureExtensions()+ "|.tbn|.dds");
}

std::string CUtil::GetSplashPath()
{
  std::array<std::string, 4> candidates {{ "special://home/media/splash.jpg", "special://home/media/splash.png", "special://xbmc/media/splash.jpg", "special://xbmc/media/splash.png" }};
  auto it = std::find_if(candidates.begin(), candidates.end(), [](std::string const& file) { return XFILE::CFile::Exists(file); });
  if (it == candidates.end())
    throw std::runtime_error("No splash image found");
  return CSpecialProtocol::TranslatePathConvertCase(*it);
}

bool CUtil::ExcludeFileOrFolder(const std::string& strFileOrFolder, const std::vector<std::string>& regexps)
{
  if (strFileOrFolder.empty())
    return false;

  CRegExp regExExcludes(true, CRegExp::autoUtf8);  // case insensitive regex

  for (const auto &regexp : regexps)
  {
    if (!regExExcludes.RegComp(regexp.c_str()))
    { // invalid regexp - complain in logs
      CLog::Log(LOGERROR, "{}: Invalid exclude RegExp:'{}'", __FUNCTION__, regexp);
      continue;
    }
    if (regExExcludes.RegFind(strFileOrFolder) > -1)
    {
      CLog::LogF(LOGDEBUG, "File '{}' excluded. (Matches exclude rule RegExp: '{}')", CURL::GetRedacted(strFileOrFolder), regexp);
      return true;
    }
  }
  return false;
}

std::string CUtil::GetFileDigest(const std::string& strPath, KODI::UTILITY::CDigest::Type type)
{
  CFile file;
  std::string result;
  if (file.Open(strPath))
  {
    CDigest digest{type};
    char temp[1024];
    while (true)
    {
      ssize_t read = file.Read(temp,1024);
      if (read <= 0)
        break;
      digest.Update(temp,read);
    }
    result = digest.Finalize();
    file.Close();
  }

  return result;
}

void CUtil::GetDVDDriveIcon(const std::string& strPath, std::string& strIcon)
{
  if (!CServiceBroker::GetMediaManager().IsDiscInDrive(strPath))
  {
    strIcon = "DefaultDVDEmpty.png";
    return ;
  }

  CFileItem item = CFileItem(strPath, false);

  if (item.IsBluray())
  {
    strIcon = "DefaultBluray.png";
    return;
  }

  if ( URIUtils::IsDVD(strPath) )
  {
    strIcon = "DefaultDVDFull.png";
    return ;
  }

  if ( URIUtils::IsISO9660(strPath) )
  {
#ifdef HAS_OPTICAL_DRIVE
    CCdInfo* pInfo = CServiceBroker::GetMediaManager().GetCdInfo();
    if ( pInfo != NULL && pInfo->IsVideoCd( 1 ) )
    {
      strIcon = "DefaultVCD.png";
      return ;
    }
#endif
    strIcon = "DefaultDVDRom.png";
    return ;
  }

  if ( URIUtils::IsCDDA(strPath) )
  {
    strIcon = "DefaultCDDA.png";
    return ;
  }
}

void CUtil::RemoveTempFiles()
{
  const std::shared_ptr<CProfileManager> profileManager = CServiceBroker::GetSettingsComponent()->GetProfileManager();

  std::string searchPath = profileManager->GetDatabaseFolder();
  CFileItemList items;
  if (!XFILE::CDirectory::GetDirectory(searchPath, items, ".tmp", DIR_FLAG_NO_FILE_DIRS))
    return;

  for (const auto &item : items)
  {
    if (item->m_bIsFolder)
      continue;
    XFILE::CFile::Delete(item->GetPath());
  }
}

/*!
  \brief Finds next unused filename that matches padded int format identifier provided
  \param[in]  fn_template    filename template consisting of a padded int format identifier (eg screenshot%03d)
  \param[in]  max            maximum number to search for available name
  \return "" on failure, string next available name matching format identifier on success
*/

std::string CUtil::GetNextFilename(const std::string &fn_template, int max)
{
  std::string searchPath = URIUtils::GetDirectory(fn_template);
  std::string mask = URIUtils::GetExtension(fn_template);
  std::string name = StringUtils::Format(fn_template, 0);

  CFileItemList items;
  if (!CDirectory::GetDirectory(searchPath, items, mask, DIR_FLAG_NO_FILE_DIRS))
    return name;

  items.SetFastLookup(true);
  for (int i = 0; i <= max; i++)
  {
    std::string name = StringUtils::Format(fn_template, i);
    if (!items.Get(name))
      return name;
  }
  return "";
}

std::string CUtil::MakeLegalFileName(std::string strFile, int LegalType)
{
  StringUtils::Replace(strFile, '/', '_');
  StringUtils::Replace(strFile, '\\', '_');
  StringUtils::Replace(strFile, '?', '_');

  if (LegalType == LEGAL_WIN32_COMPAT)
  {
    // just filter out some illegal characters on windows
    StringUtils::Replace(strFile, ':', '_');
    StringUtils::Replace(strFile, '*', '_');
    StringUtils::Replace(strFile, '?', '_');
    StringUtils::Replace(strFile, '\"', '_');
    StringUtils::Replace(strFile, '<', '_');
    StringUtils::Replace(strFile, '>', '_');
    StringUtils::Replace(strFile, '|', '_');
    StringUtils::TrimRight(strFile, ". ");
  }
  return strFile;
}

// legalize entire path
std::string CUtil::MakeLegalPath(std::string strPathAndFile, int LegalType)
{
  if (URIUtils::IsStack(strPathAndFile))
    return MakeLegalPath(CStackDirectory::GetFirstStackedFile(strPathAndFile));
  if (URIUtils::IsMultiPath(strPathAndFile))
    return MakeLegalPath(CMultiPathDirectory::GetFirstPath(strPathAndFile));
  if (!URIUtils::IsHD(strPathAndFile) && !URIUtils::IsSmb(strPathAndFile) && !URIUtils::IsNfs(strPathAndFile))
    return strPathAndFile; // we don't support writing anywhere except HD, SMB and NFS - no need to legalize path

  bool trailingSlash = URIUtils::HasSlashAtEnd(strPathAndFile);
  std::vector<std::string> dirs = URIUtils::SplitPath(strPathAndFile);
  if (dirs.empty())
    return strPathAndFile;
  // we just add first token to path and don't legalize it - possible values:
  // "X:" (local win32), "" (local unix - empty string before '/') or
  // "protocol://domain"
  std::string dir(dirs.front());
  URIUtils::AddSlashAtEnd(dir);
  for (std::vector<std::string>::const_iterator it = dirs.begin() + 1; it != dirs.end(); ++it)
    dir = URIUtils::AddFileToFolder(dir, MakeLegalFileName(*it, LegalType));
  if (trailingSlash) URIUtils::AddSlashAtEnd(dir);
  return dir;
}

std::string CUtil::ValidatePath(std::string path, bool bFixDoubleSlashes /* = false */)
{

  // Don't do any stuff on URLs containing %-characters or protocols that embed
  // filenames. NOTE: Don't use IsInZip or IsInRar here since it will infinitely
  // recurse and crash XBMC
  if (URIUtils::IsURL(path) &&
      (path.find('%') != std::string::npos ||
      StringUtils::StartsWithNoCase(path, "apk:") ||
      StringUtils::StartsWithNoCase(path, "zip:") ||
      StringUtils::StartsWithNoCase(path, "rar:") ||
      StringUtils::StartsWithNoCase(path, "stack:") ||
      StringUtils::StartsWithNoCase(path, "bluray:") ||
      StringUtils::StartsWithNoCase(path, "multipath:") ))
    return path;

    // check the path for incorrect slashes
#if defined(TARGET_WINDOWS) || defined(_XBOX)
  if (URIUtils::IsDOSPath(path))
  {
    StringUtils::Replace(path, '/', '\\');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes && !path.empty())
    {
      // Fixup for double back slashes (but ignore the \\ of unc-paths)
      for (size_t x = 1; x < path.size() - 1; x++)
      {
        if (path[x] == '\\' && path[x + 1] == '\\')
          path.erase(x, 1);
      }
    }
  }
  else if (path.find("://") != std::string::npos || path.find(":\\\\") != std::string::npos)
#endif
  {
    StringUtils::Replace(path, '\\', '/');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes && !path.empty())
    {
      // Fixup for double forward slashes(/) but don't touch the :// of URLs
      for (size_t x = 2; x < path.size() - 1; x++)
      {
        if (path[x] == '/' && path[x + 1] == '/' &&
            !(path[x - 1] == ':' || (path[x - 1] == '/' && path[x - 2] == ':')))
          path.erase(x, 1);
      }
    }
  }
  return path;
}

void CUtil::SplitParams(const std::string &paramString, std::vector<std::string> &parameters)
{
  bool inQuotes = false;
  bool lastEscaped = false; // only every second character can be escaped
  int inFunction = 0;
  size_t whiteSpacePos = 0;
  std::string parameter;
  parameters.clear();
  for (size_t pos = 0; pos < paramString.size(); pos++)
  {
    char ch = paramString[pos];
    bool escaped = (pos > 0 && paramString[pos - 1] == '\\' && !lastEscaped);
    lastEscaped = escaped;
    if (inQuotes)
    { // if we're in a quote, we accept everything until the closing quote
      if (ch == '"' && !escaped)
      { // finished a quote - no need to add the end quote to our string
        inQuotes = false;
      }
    }
    else
    { // not in a quote, so check if we should be starting one
      if (ch == '"' && !escaped)
      { // start of quote - no need to add the quote to our string
        inQuotes = true;
      }
      if (inFunction && ch == ')')
      { // end of a function
        inFunction--;
      }
      if (ch == '(')
      { // start of function
        inFunction++;
      }
      if (!inFunction && ch == ',')
      { // not in a function, so a comma signifies the end of this parameter
        if (whiteSpacePos)
          parameter.resize(whiteSpacePos);
        // trim off start and end quotes
        if (parameter.length() > 1 && parameter[0] == '"' && parameter[parameter.length() - 1] == '"')
          parameter = parameter.substr(1, parameter.length() - 2);
        else if (parameter.length() > 3 && parameter[parameter.length() - 1] == '"')
        {
          // check name="value" style param.
          size_t quotaPos = parameter.find('"');
          if (quotaPos > 1 && quotaPos < parameter.length() - 1 && parameter[quotaPos - 1] == '=')
          {
            parameter.erase(parameter.length() - 1);
            parameter.erase(quotaPos);
          }
        }
        parameters.push_back(parameter);
        parameter.clear();
        whiteSpacePos = 0;
        continue;
      }
    }
    if ((ch == '"' || ch == '\\') && escaped)
    { // escaped quote or backslash
      parameter[parameter.size()-1] = ch;
      continue;
    }
    // whitespace handling - we skip any whitespace at the left or right of an unquoted parameter
    if (ch == ' ' && !inQuotes)
    {
      if (parameter.empty()) // skip whitespace on left
        continue;
      if (!whiteSpacePos) // make a note of where whitespace starts on the right
        whiteSpacePos = parameter.size();
    }
    else
      whiteSpacePos = 0;
    parameter += ch;
  }
  if (inFunction || inQuotes)
    CLog::Log(LOGWARNING, "{}({}) - end of string while searching for ) or \"", __FUNCTION__,
              paramString);
  if (whiteSpacePos)
    parameter.erase(whiteSpacePos);
  // trim off start and end quotes
  if (parameter.size() > 1 && parameter[0] == '"' && parameter[parameter.size() - 1] == '"')
    parameter = parameter.substr(1,parameter.size() - 2);
  else if (parameter.size() > 3 && parameter[parameter.size() - 1] == '"')
  {
    // check name="value" style param.
    size_t quotaPos = parameter.find('"');
    if (quotaPos > 1 && quotaPos < parameter.length() - 1 && parameter[quotaPos - 1] == '=')
    {
      parameter.erase(parameter.length() - 1);
      parameter.erase(quotaPos);
    }
  }
  if (!parameter.empty() || parameters.size())
    parameters.push_back(parameter);
}

int CUtil::GetMatchingSource(const std::string& strPath1, VECSOURCES& VECSOURCES, bool& bIsSourceName)
{
  if (strPath1.empty())
    return -1;

  // copy as we may change strPath
  std::string strPath = strPath1;

  // Check for special protocols
  CURL checkURL(strPath);

  if (StringUtils::StartsWith(strPath, "special://skin/"))
    return 1;

  // do not return early if URL protocol is "plugin"
  // since video- and/or audio-plugins can be configured as mediasource

  // stack://
  if (checkURL.IsProtocol("stack"))
    strPath.erase(0, 8); // remove the stack protocol

  if (checkURL.IsProtocol("shout"))
    strPath = checkURL.GetHostName();

  if (checkURL.IsProtocol("multipath"))
    strPath = CMultiPathDirectory::GetFirstPath(strPath);

  bIsSourceName = false;
  int iIndex = -1;

  // we first test the NAME of a source
  for (int i = 0; i < (int)VECSOURCES.size(); ++i)
  {
    const CMediaSource &share = VECSOURCES[i];
    std::string strName = share.strName;

    // special cases for dvds
    if (URIUtils::IsOnDVD(share.strPath))
    {
      if (URIUtils::IsOnDVD(strPath))
        return i;

      // not a path, so we need to modify the source name
      // since we add the drive status and disc name to the source
      // "Name (Drive Status/Disc Name)"
      size_t iPos = strName.rfind('(');
      if (iPos != std::string::npos && iPos > 1)
        strName.resize(iPos - 1);
    }
    if (StringUtils::EqualsNoCase(strPath, strName))
    {
      bIsSourceName = true;
      return i;
    }
  }

  // now test the paths

  // remove user details, and ensure path only uses forward slashes
  // and ends with a trailing slash so as not to match a substring
  CURL urlDest(strPath);
  urlDest.SetOptions("");
  urlDest.SetProtocolOptions("");
  std::string strDest = urlDest.GetWithoutUserDetails();
  ForceForwardSlashes(strDest);
  if (!URIUtils::HasSlashAtEnd(strDest))
    strDest += "/";

  size_t iLength = 0;
  size_t iLenPath = strDest.size();
  for (int i = 0; i < (int)VECSOURCES.size(); ++i)
  {
    const CMediaSource &share = VECSOURCES[i];

    // does it match a source name?
    if (share.strPath.substr(0,8) == "shout://")
    {
      CURL url(share.strPath);
      if (strPath == url.GetHostName())
        return i;
    }

    // doesn't match a name, so try the source path
    std::vector<std::string> vecPaths;

    // add any concatenated paths if they exist
    if (!share.vecPaths.empty())
      vecPaths = share.vecPaths;

    // add the actual share path at the front of the vector
    vecPaths.insert(vecPaths.begin(), share.strPath);

    // test each path
    for (const auto &path : vecPaths)
    {
      // remove user details, and ensure path only uses forward slashes
      // and ends with a trailing slash so as not to match a substring
      CURL urlShare(path);
      urlShare.SetOptions("");
      urlShare.SetProtocolOptions("");
      std::string strShare = urlShare.GetWithoutUserDetails();
      ForceForwardSlashes(strShare);
      if (!URIUtils::HasSlashAtEnd(strShare))
        strShare += "/";
      size_t iLenShare = strShare.size();

      if ((iLenPath >= iLenShare) && StringUtils::StartsWithNoCase(strDest, strShare) && (iLenShare > iLength))
      {
        // if exact match, return it immediately
        if (iLenPath == iLenShare)
        {
          // if the path EXACTLY matches an item in a concatenated path
          // set source name to true to load the full virtualpath
          bIsSourceName = false;
          if (vecPaths.size() > 1)
            bIsSourceName = true;
          return i;
        }
        iIndex = i;
        iLength = iLenShare;
      }
    }
  }

  // return the index of the share with the longest match
  if (iIndex == -1)
  {

    // rar:// and zip://
    // if archive wasn't mounted, look for a matching share for the archive instead
    if( StringUtils::StartsWithNoCase(strPath, "rar://") || StringUtils::StartsWithNoCase(strPath, "zip://") )
    {
      // get the hostname portion of the url since it contains the archive file
      strPath = checkURL.GetHostName();

      bIsSourceName = false;
      bool bDummy;
      return GetMatchingSource(strPath, VECSOURCES, bDummy);
    }

    CLog::Log(LOGDEBUG, "CUtil::GetMatchingSource: no matching source found for [{}]", strPath1);
  }
  return iIndex;
}

std::string CUtil::TranslateSpecialSource(const std::string &strSpecial)
{
  if (!strSpecial.empty() && strSpecial[0] == '$')
  {
    if (StringUtils::StartsWithNoCase(strSpecial, "$home"))
      return URIUtils::AddFileToFolder("special://home/", strSpecial.substr(5));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$subtitles"))
      return URIUtils::AddFileToFolder("special://subtitles/", strSpecial.substr(10));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$userdata"))
      return URIUtils::AddFileToFolder("special://userdata/", strSpecial.substr(9));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$database"))
      return URIUtils::AddFileToFolder("special://database/", strSpecial.substr(9));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$thumbnails"))
      return URIUtils::AddFileToFolder("special://thumbnails/", strSpecial.substr(11));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$recordings"))
      return URIUtils::AddFileToFolder("special://recordings/", strSpecial.substr(11));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$screenshots"))
      return URIUtils::AddFileToFolder("special://screenshots/", strSpecial.substr(12));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$musicplaylists"))
      return URIUtils::AddFileToFolder("special://musicplaylists/", strSpecial.substr(15));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$videoplaylists"))
      return URIUtils::AddFileToFolder("special://videoplaylists/", strSpecial.substr(15));
    else if (StringUtils::StartsWithNoCase(strSpecial, "$cdrips"))
      return URIUtils::AddFileToFolder("special://cdrips/", strSpecial.substr(7));
    // this one will be removed post 2.0
    else if (StringUtils::StartsWithNoCase(strSpecial, "$playlists"))
      return URIUtils::AddFileToFolder(CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_SYSTEM_PLAYLISTSPATH), strSpecial.substr(10));
  }
  return strSpecial;
}

std::string CUtil::MusicPlaylistsLocation()
{
  const std::string path = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_SYSTEM_PLAYLISTSPATH);
  std::vector<std::string> vec;
  vec.push_back(URIUtils::AddFileToFolder(path, "music"));
  vec.push_back(URIUtils::AddFileToFolder(path, "mixed"));
  return XFILE::CMultiPathDirectory::ConstructMultiPath(vec);
}

std::string CUtil::VideoPlaylistsLocation()
{
  const std::string path = CServiceBroker::GetSettingsComponent()->GetSettings()->GetString(CSettings::SETTING_SYSTEM_PLAYLISTSPATH);
  std::vector<std::string> vec;
  vec.push_back(URIUtils::AddFileToFolder(path, "video"));
  vec.push_back(URIUtils::AddFileToFolder(path, "mixed"));
  return XFILE::CMultiPathDirectory::ConstructMultiPath(vec);
}

void CUtil::DeleteMusicDatabaseDirectoryCache()
{
  CUtil::DeleteDirectoryCache("mdb-");
  CUtil::DeleteDirectoryCache("sp-"); // overkill as it will delete video smartplaylists, but as we can't differentiate based on URL...
}

void CUtil::DeleteVideoDatabaseDirectoryCache()
{
  CUtil::DeleteDirectoryCache("vdb-");
  CUtil::DeleteDirectoryCache("sp-"); // overkill as it will delete music smartplaylists, but as we can't differentiate based on URL...
}

void CUtil::DeleteDirectoryCache(const std::string &prefix)
{
  std::string searchPath = "special://temp/";
  CFileItemList items;
  if (!XFILE::CDirectory::GetDirectory(searchPath, items, ".fi", DIR_FLAG_NO_FILE_DIRS))
    return;

  for (const auto &item : items)
  {
    if (item->m_bIsFolder)
      continue;
    std::string fileName = URIUtils::GetFileName(item->GetPath());
    if (StringUtils::StartsWith(fileName, prefix))
      XFILE::CFile::Delete(item->GetPath());
  }
}

double CUtil::AlbumRelevance(const std::string& strAlbumTemp1, const std::string& strAlbum1, const std::string& strArtistTemp1, const std::string& strArtist1)
{
  // case-insensitive fuzzy string comparison on the album and artist for relevance
  // weighting is identical, both album and artist are 50% of the total relevance
  // a missing artist means the maximum relevance can only be 0.50
  std::string strAlbumTemp = strAlbumTemp1;
  StringUtils::ToLower(strAlbumTemp);
  std::string strAlbum = strAlbum1;
  StringUtils::ToLower(strAlbum);
  double fAlbumPercentage = fstrcmp(strAlbumTemp.c_str(), strAlbum.c_str());
  double fArtistPercentage = 0.0;
  if (!strArtist1.empty())
  {
    std::string strArtistTemp = strArtistTemp1;
    StringUtils::ToLower(strArtistTemp);
    std::string strArtist = strArtist1;
    StringUtils::ToLower(strArtist);
    fArtistPercentage = fstrcmp(strArtistTemp.c_str(), strArtist.c_str());
  }
  double fRelevance = fAlbumPercentage * 0.5 + fArtistPercentage * 0.5;
  return fRelevance;
}

bool CUtil::MakeShortenPath(std::string StrInput, std::string& StrOutput, size_t iTextMaxLength)
{
  size_t iStrInputSize = StrInput.size();
  if(iStrInputSize <= 0 || iTextMaxLength >= iStrInputSize)
  {
    StrOutput = StrInput;
    return true;
  }

  char cDelim = '\0';
  size_t nGreaterDelim, nPos;

  nPos = StrInput.find_last_of( '\\' );
  if (nPos != std::string::npos)
    cDelim = '\\';
  else
  {
    nPos = StrInput.find_last_of( '/' );
    if (nPos != std::string::npos)
      cDelim = '/';
  }
  if ( cDelim == '\0' )
    return false;

  if (nPos == StrInput.size() - 1)
  {
    StrInput.erase(StrInput.size() - 1);
    nPos = StrInput.find_last_of(cDelim);
  }
  while( iTextMaxLength < iStrInputSize )
  {
    nPos = StrInput.find_last_of( cDelim, nPos );
    nGreaterDelim = nPos;

    if (nPos == std::string::npos || nPos == 0)
      break;

    nPos = StrInput.find_last_of( cDelim, nPos - 1 );

    if ( nPos == std::string::npos )
      break;
    if ( nGreaterDelim > nPos )
      StrInput.replace( nPos + 1, nGreaterDelim - nPos - 1, ".." );
    iStrInputSize = StrInput.size();
  }
  // replace any additional /../../ with just /../ if necessary
  std::string replaceDots = StringUtils::Format("..{}..", cDelim);
  while (StrInput.size() > (unsigned int)iTextMaxLength)
    if (!StringUtils::Replace(StrInput, replaceDots, ".."))
      break;
  // finally, truncate our string to force inside our max text length,
  // replacing the last 2 characters with ".."

  // eg end up with:
  // "smb://../Playboy Swimsuit Cal.."
  if (iTextMaxLength > 2 && StrInput.size() > (unsigned int)iTextMaxLength)
  {
    StrInput.erase(iTextMaxLength - 2);
    StrInput += "..";
  }
  StrOutput = StrInput;
  return true;
}

bool CUtil::SupportsWriteFileOperations(const std::string& strPath)
{
  // currently only hd, smb, nfs and dav support delete and rename
  if (URIUtils::IsHD(strPath))
    return true;
  if (URIUtils::IsSmb(strPath))
    return true;
  if (URIUtils::IsNfs(strPath))
    return true;
  if (URIUtils::IsDAV(strPath))
    return true;
  if (URIUtils::IsStack(strPath))
    return SupportsWriteFileOperations(CStackDirectory::GetFirstStackedFile(strPath));
  if (URIUtils::IsMultiPath(strPath))
    return CMultiPathDirectory::SupportsWriteFileOperations(strPath);

  return false;
}

bool CUtil::SupportsReadFileOperations(const std::string& strPath)
{
  return !URIUtils::IsVideoDb(strPath);
}

std::string CUtil::GetDefaultFolderThumb(const std::string &folderThumb)
{
  if (CServiceBroker::GetGUI()->GetTextureManager().HasTexture(folderThumb))
    return folderThumb;
  return "";
}

void CUtil::GetSkinThemes(std::vector<std::string>& vecTheme)
{
#ifdef _XBOX
  static const std::string TexturesXbt = "Textures.xpr";
#else
  static const std::string TexturesXbt = "Textures.xbt";
#endif

  std::string strPath = URIUtils::AddFileToFolder(CServiceBroker::GetWinSystem()->GetGfxContext().GetMediaDir(), "media");
  CFileItemList items;
  CDirectory::GetDirectory(strPath, items, "", DIR_FLAG_DEFAULTS);
  // Search for Themes in the Current skin!
  for (const auto &pItem : items)
  {
    if (!pItem->m_bIsFolder)
    {
      std::string strExtension = URIUtils::GetExtension(pItem->GetPath());
      std::string strLabel = pItem->GetLabel();
#ifdef _XBOX
      if ((strExtension == ".xpr" && !StringUtils::EqualsNoCase(strLabel, TexturesXbt)))
#else
      if ((strExtension == ".xbt" && !StringUtils::EqualsNoCase(strLabel, TexturesXbt)))
#endif
        vecTheme.push_back(StringUtils::Left(strLabel, strLabel.size() - strExtension.size()));
    }
    else
    {
      // check if this is an xbt:// VFS path
      CURL itemUrl(pItem->GetPath());
#ifdef _XBOX
      if (!itemUrl.IsProtocol("xpr") || !itemUrl.GetFileName().empty())
#else
      if (!itemUrl.IsProtocol("xbt") || !itemUrl.GetFileName().empty())
#endif
        continue;

      std::string strLabel = URIUtils::GetFileName(itemUrl.GetHostName());
      if (!StringUtils::EqualsNoCase(strLabel, TexturesXbt))
        vecTheme.push_back(StringUtils::Left(strLabel, strLabel.size() - URIUtils::GetExtension(strLabel).size()));
    }
  }
  std::sort(vecTheme.begin(), vecTheme.end(), sortstringbyname());
}

void CUtil::InitRandomSeed()
{
  // Init random seed
  auto now = std::chrono::steady_clock::now();
  auto seed = now.time_since_epoch();

  srand(static_cast<unsigned int>(seed.count()));
}

int CUtil::LookupRomanDigit(char roman_digit)
{
  switch (roman_digit)
  {
    case 'i':
    case 'I':
      return 1;
    case 'v':
    case 'V':
      return 5;
    case 'x':
    case 'X':
      return 10;
    case 'l':
    case 'L':
      return 50;
    case 'c':
    case 'C':
      return 100;
    case 'd':
    case 'D':
      return 500;
    case 'm':
    case 'M':
      return 1000;
    default:
      return 0;
  }
}

int CUtil::TranslateRomanNumeral(const char* roman_numeral)
{

  int decimal = -1;

  if (roman_numeral && roman_numeral[0])
  {
    int temp_sum  = 0,
        last      = 0,
        repeat    = 0,
        trend     = 1;
    decimal = 0;
    while (*roman_numeral)
    {
      int digit = CUtil::LookupRomanDigit(*roman_numeral);
      int test  = last;

      // General sanity checks

      // numeral not in LUT
      if (!digit)
        return -1;

      while (test > 5)
        test /= 10;

      // N = 10^n may not precede (N+1) > 10^(N+1)
      if (test == 1 && digit > last * 10)
        return -1;

      // N = 5*10^n may not precede (N+1) >= N
      if (test == 5 && digit >= last)
        return -1;

      // End general sanity checks

      if (last < digit)
      {
        // smaller numerals may not repeat before a larger one
        if (repeat)
          return -1;

        temp_sum += digit;

        repeat  = 0;
        trend   = 0;
      }
      else if (last == digit)
      {
        temp_sum += digit;
        repeat++;
        trend = 1;
      }
      else
      {
        if (!repeat)
          decimal += 2 * last - temp_sum;
        else
          decimal += temp_sum;

        temp_sum = digit;

        trend   = 1;
        repeat  = 0;
      }
      // Post general sanity checks

      // numerals may not repeat more than thrice
      if (repeat == 3)
        return -1;

      last = digit;
      roman_numeral++;
    }

    if (trend)
      decimal += temp_sum;
    else
      decimal += 2 * last - temp_sum;
  }
  return decimal;
}

std::string CUtil::GetFrameworksPath(bool forPython)
{
  std::string strFrameworksPath;
#if defined(TARGET_DARWIN)
  strFrameworksPath = CDarwinUtils::GetFrameworkPath(forPython);
#endif
  return strFrameworksPath;
}

void CUtil::GetRecursiveListing(const std::string& strPath, CFileItemList& items, const std::string& strMask, unsigned int flags /* = DIR_FLAG_DEFAULTS */)
{
  CFileItemList myItems;
  CDirectory::GetDirectory(strPath,myItems,strMask,flags);
  for (const auto &item : myItems)
  {
    if (item->m_bIsFolder)
      CUtil::GetRecursiveListing(item->GetPath(),items,strMask,flags);
    else
      items.Add(item);
  }
}

void CUtil::GetRecursiveDirsListing(const std::string& strPath, CFileItemList& item, unsigned int flags /* = DIR_FLAG_DEFAULTS */)
{
  CFileItemList myItems;
  CDirectory::GetDirectory(strPath,myItems,"",flags);
  for (const auto &i : myItems)
  {
    if (i->m_bIsFolder && !i->IsPath(".."))
    {
      item.Add(i);
      CUtil::GetRecursiveDirsListing(i->GetPath(),item,flags);
    }
  }
}

void CUtil::ForceForwardSlashes(std::string& strPath)
{
  size_t iPos = strPath.rfind('\\');
  while (iPos != std::string::npos)
  {
    strPath.at(iPos) = '/';
    iPos = strPath.rfind('\\');
  }
}

int CUtil::GetRandomNumber()
{
#if !defined(TARGET_WINDOWS) && !defined(_XBOX)
  return rand_r(&s_randomSeed);
#else
  unsigned int number;
  if (rand_s(&number) == 0)
    return (int)number;

  return rand();
#endif
}

void CUtil::CopyUserDataIfNeeded(const std::string& strPath,
                                 const std::string& file,
                                 const std::string& destname)
{
  std::string destPath;
  if (destname.empty())
    destPath = URIUtils::AddFileToFolder(strPath, file);
  else
    destPath = URIUtils::AddFileToFolder(strPath, destname);

  if (!CFile::Exists(destPath))
  {
    // need to copy it across
    std::string srcPath = URIUtils::AddFileToFolder("special://xbmc/userdata/", file);
    CFile::Copy(srcPath, destPath);
  }
}
