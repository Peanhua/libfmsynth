#ifndef TYPES_HH_
#define TYPES_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif


#include <json11.hpp>


#ifdef HAVE_FMT
# include <fmt/format.h>
using fmt::format;
#else
# include <format>
using std::format;
#endif


#endif
