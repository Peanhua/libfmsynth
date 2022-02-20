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
#include "Util.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <json11.hpp>
#include "QtIncludeBegin.hh"
#include <QtCore/QStandardPaths>
#include "QtIncludeEnd.hh"


Settings * UserSettings = nullptr;

Settings::Settings()
  : _dirty(false)
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
  _ints["window_x"] = -1;
  _ints["window_y"] = -1;
  _ints["window_width"] = -1;
  _ints["window_height"] = -1;
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
  if(!_dirty)
    return;

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
  _dirty = false;
}


void Settings::Load()
{
  std::cout << "Loading settings from '" << _filename << "'" << std::endl;
  auto [json_ptr, error] = fmsynth::util::LoadJsonFile(_filename);
  if(!json_ptr)
    {
      std::cerr << error << std::endl;
      return;
    }

  auto json = *json_ptr;

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

  _dirty = false;
}


void Settings::Set(const std::string & name, bool value)
{
  if(_bools[name] != value)
    _dirty = true;
  _bools[name] = value;
}


void Settings::Set(const std::string & name, int value)
{
  if(_ints[name] != value)
    _dirty = true;
  _ints[name] = value;
}


void Settings::Set(const std::string & name, double value)
{
  if(_doubles[name] != value)
    _dirty = true;
  _doubles[name] = value;
}


void Settings::Set(const std::string & name, const std::string & value)
{
  if(_strings[name] != value)
    _dirty = true;
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
