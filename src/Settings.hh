#ifndef SETTINGS_HH_
#define SETTINGS_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <map>
#include <string>


class Settings
{
public:
  Settings();

  void Save();
  void Load();

  void SetDefaults();
  
  void Set(const std::string & name, bool value);
  void Set(const std::string & name, int value);
  void Set(const std::string & name, double value);
  void Set(const std::string & name, const std::string & value);

  [[nodiscard]] bool                GetBool(const std::string & name)   const;
  [[nodiscard]] int                 GetInt(const std::string & name)    const;
  [[nodiscard]] double              GetDouble(const std::string & name) const;
  [[nodiscard]] const std::string & GetString(const std::string & name) const;
  
private:
  std::string                        _filename;
  bool                               _dirty;
  std::map<std::string, bool>        _bools;
  std::map<std::string, int>         _ints;
  std::map<std::string, double>      _doubles;
  std::map<std::string, std::string> _strings;
};

extern Settings * UserSettings;

#endif
