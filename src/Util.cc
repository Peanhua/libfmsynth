/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Util.hh"
#include <fstream>
#include <iostream>

using namespace fmsynth::util;


std::tuple<bool, std::string> fmsynth::util::LoadText(const std::string & filename)
{
  std::string text;
  std::ifstream fp(filename);
  if(!fp)
    return { false, "Failed to open '" + filename + "' for reading." };
  
  std::string tmp;
  while(std::getline(fp, tmp))
    text += tmp + '\n';
      
  // todo: Add proper error message.
  if(!fp.eof())
    return { false, "Error loading '" + filename + "': something went wrong." };
  
  return { true, text };
}


std::tuple<json11::Json *, std::string> fmsynth::util::LoadJson(const std::string & json_string)
{
  auto json = new json11::Json();
  if(!json_string.empty())
    {
      std::string err;
      *json = json11::Json::parse(json_string, err);
      if(!json->is_object())
        {
          delete json;
          return { nullptr, "Error while parsing text to json: " + err };
        }
    }
  return { json, "" };
}


std::tuple<json11::Json *, std::string> fmsynth::util::LoadJsonFile(const std::string & filename)
{
  auto [success, text] = LoadText(filename);
  if(!success)
    return { nullptr, text };

  return LoadJson(text);
}

