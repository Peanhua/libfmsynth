/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wsign-conversion"
# pragma clang diagnostic ignored "-Wfloat-conversion"
# pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
# pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wuseless-cast"
# pragma GCC diagnostic ignored "-Wsign-conversion"
# pragma GCC diagnostic ignored "-Wconversion"
# pragma GCC diagnostic ignored "-Wcast-align"
# pragma GCC diagnostic ignored "-Wfloat-conversion"
# pragma GCC diagnostic ignored "-Wclass-memaccess"
# pragma GCC diagnostic ignored "-Wdeprecated-copy"
# pragma GCC diagnostic ignored "-Wpadded"
# if __GNUC__ >= 11
#  pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"
# endif
#endif
