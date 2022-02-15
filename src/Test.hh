#ifndef TEST_HH_
#define TEST_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <iostream>
#include <cassert>

#define testAssert(ID, NAME, CONDITION) do {                            \
  auto testAssertId = ID;                                               \
  assert(testAssertId > 0);                                             \
  if(CONDITION)                                                         \
    std::cout << "ok " << testAssertId << " " << NAME << std::endl;     \
  else                                                                  \
    {                                                                   \
      std::cout << "not ok " << testAssertId << " " << NAME << std::endl; \
      std::cout << "  failed condition: " << #CONDITION << std::endl;   \
    }                                                                   \
  } while(0)

#define testSkip(ID, NAME, DESCRIPTION) do {                            \
    std::cout << "ok " << ID << " " << NAME                             \
              << " # SKIP " << DESCRIPTION << std::endl;                \
  } while(0)

#define testPlan(TESTCOUNT) do { std::cout << "1.." << TESTCOUNT << std::endl; } while(0)


static int Test();

static std::string srcdir;

int main()
{
  srcdir = std::getenv("srcdir");
  
  auto next_id = Test();
  testPlan(next_id - 1);
  return 0;
}

#endif
