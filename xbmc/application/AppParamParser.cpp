/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "AppParamParser.h"

#include "FileItem.h"
#include "application/AppParams.h"

CAppParamParser::CAppParamParser() : m_params(std::make_shared<CAppParams>())
{
}

void CAppParamParser::Parse(const char* const* argv, int nArgs)
{
  std::vector<std::string> args;
  args.reserve(nArgs);

  for (int i = 0; i < nArgs; i++)
  {
    args.emplace_back(argv[i]);
    if (i > 0)
      ParseArg(argv[i]);
  }

  // Record raw paramerters
  m_params->SetRawArgs(std::move(args));
}

void CAppParamParser::ParseArg(const std::string &arg)
{
  if (arg == "-fs" || arg == "--fullscreen")
    m_params->SetStartFullScreen(true);
  else if (arg == "--standalone")
    m_params->SetStandAlone(true);
}
