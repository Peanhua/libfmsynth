/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Settings.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <json11.hpp>
#include "QtIncludeBegin.hh"
#include <QtCore/QStandardPaths>
#include "QtIncludeEnd.hh"


Settings * UserSettings = nullptr;

Settings::Settings()
{
  SetDefaults();
  _filename = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
  _filename += "/fmsedit.json";
}

void Settings::SetDefaults()
{
  _ints["theme"] = 0;
  _ints["recent_files"] = 0;
  _ints["sample_rate"] = 44100;
  _bools["nodecategory_inputs_collapsed"]        = false;
  _bools["nodecategory_inputs_grid"]             = false;
  _bools["nodecategory_oscillators_collapsed"]   = false;
  _bools["nodecategory_oscillators_grid"]        = false;
  _bools["nodecategory_envelopes_collapsed"]     = false;
  _bools["nodecategory_envelopes_grid"]          = false;
  _bools["nodecategory_filters_collapsed"]       = false;
  _bools["nodecategory_filters_grid"]            = false;
  _bools["nodecategory_math_collapsed"]          = false;
  _bools["nodecategory_math_grid"]               = false;
  _bools["nodecategory_outputs_collapsed"]       = false;
  _bools["nodecategory_outputs_grid"]            = false;
  _bools["nodecategory_miscellaneous_collapsed"] = false;
  _bools["nodecategory_miscellaneous_grid"]      = false;
  _bools["snap_to_grid"] = false;
}


void Settings::Save()
{
  std::ofstream file(_filename);
  if(!file)
    { // todo: Show the error in the GUI.
      std::cerr << "Error while saving: something went wrong\n";
      return;
    }

  json11::Json settings {
    json11::Json::object {
      { "bools",   _bools   },
      { "ints",    _ints    },
      { "doubles", _doubles },
      { "strings", _strings }
    }
  };

  file << settings.dump();
  file << std::flush;
  // todo: Check for failure.
  // todo: Show the message in the GUI.
  std::cout << "Saved settings " << _filename << "\n";  
}


void Settings::Load()
{
  auto LoadText = [this]()
  {
    std::string text;
    std::ifstream fp(_filename);
    if(fp)
      {
        std::string tmp;
        while(std::getline(fp, tmp))
          text += tmp + '\n';

        if(!fp.eof())
          std::cerr << "Error while reading '" << _filename << "': something went wrong" << std::endl;
      }
    return text;
  };

  std::cout << "Loading settings from '" << _filename << "'" << std::endl;
  auto json_string = LoadText();
  if(json_string.empty())
    return;

  std::string err;
  auto json = json11::Json::parse(json_string, err);
  if(!json.is_object())
    {
      std::cerr << "Error while parsing: " << err << std::endl;
      return;
    }

  auto bools = json["bools"].object_items();
  for(auto & [k, v] : bools)
    _bools[k] = v.bool_value();
  
  auto ints = json["ints"].object_items();
  for(auto & [k, v] : ints)
    _ints[k] = v.int_value();

  auto doubles = json["doubles"].object_items();
  for(auto & [k, v] : doubles)
    _doubles[k] = v.number_value();

  auto strings = json["strings"].object_items();
  for(auto & [k, v] : strings)
    _strings[k] = v.string_value();
}


void Settings::Set(const std::string & name, bool value)
{
  _bools[name] = value;
}


void Settings::Set(const std::string & name, int value)
{
  _ints[name] = value;
}


void Settings::Set(const std::string & name, double value)
{
  _doubles[name] = value;
}


void Settings::Set(const std::string & name, const std::string & value)
{
  _strings[name] = value;
}


bool Settings::GetBool(const std::string & name) const
{
  auto it = _bools.find(name);
  if(it != _bools.end())
    return (*it).second;

  assert(false);
  return false;
}

int Settings::GetInt(const std::string & name) const
{
  auto it = _ints.find(name);
  if(it != _ints.end())
    return (*it).second;

  assert(false);
  return 0;
}


double Settings::GetDouble(const std::string & name) const
{
  auto it = _doubles.find(name);
  if(it != _doubles.end())
    return (*it).second;

  assert(false);
  return 0;
}


const std::string & Settings::GetString(const std::string & name) const
{
  auto it = _strings.find(name);
  if(it != _strings.end())
    return (*it).second;

  assert(false);
  return name;
}
