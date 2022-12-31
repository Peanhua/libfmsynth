#ifndef STD_FORMAT_HH_
#define STD_FORMAT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "../config.h"
#if HAVE_FMT
# include <fmt/core.h>
  using fmt::format;
#else
# if __has_cpp_attribute(_cpp_lib_format)
#  include <format>
   using std::format;
# else
#  error "Unable to detect format library."
# endif
#endif

#endif
