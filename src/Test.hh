#ifndef TEST_HH_
#define TEST_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <iostream>
#include <cassert>

#define testAssert(NAME, CONDITION) do {                                \
    testid++;                                                           \
    if(CONDITION)                                                       \
      std::cout << "ok " << testid << " " << (NAME) << std::endl;       \
    else                                                                \
    {                                                                   \
      std::cout << "not ok " << testid << " " << (NAME) << std::endl;   \
      std::cout << "  failed condition: " << #CONDITION << std::endl;   \
    }                                                                   \
  } while(0)

#define testSkip(NAME, DESCRIPTION) do {                                \
    testid++;                                                           \
    std::cout << "ok " << testid << " " << (NAME)                       \
              << " # SKIP " << (DESCRIPTION) << std::endl;              \
  } while(0)

#define testComment std::cout << "# "


static void Test();

static std::string srcdir;
static int testid;

int main()
{
  auto sd = std::getenv("srcdir");
  if(sd)
    srcdir = sd;
  else
    srcdir = "src";
  
  testid = 0;
  
  Test();

  std::cout << "1.." << testid << std::endl;
  
  return 0;
}


#define FloatEqual(a, b, tolerance) (std::abs((a) - (b)) <= tolerance)

#endif
