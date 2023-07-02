#ifndef UTIL_HH_
#define UTIL_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <string>
#include <tuple>
#include <json11.hpp>

namespace fmsynth::util
{
  [[nodiscard]] extern std::tuple<bool, std::string>           LoadText(const std::string & filename); // Returned string contains error message upon failure.
  [[nodiscard]] extern std::tuple<json11::Json *, std::string> LoadJson(const std::string & json_string);
  [[nodiscard]] extern std::tuple<json11::Json *, std::string> LoadJsonFile(const std::string & filename);
};


#endif
