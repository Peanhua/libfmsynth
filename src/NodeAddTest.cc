/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Test.hh"
#include "NodeAdd.hh"

static void Test()
{
  {
    fmsynth::NodeAdd node;
    node.SetSamplesPerSecond(300);
    node.AddFormInputNode(nullptr);
    node.SetValue(3);

    unsigned int ind = 0;
    std::array values
      {
        -99999999.0,
        -1.0,
        -0.321,
        -0.01,
        0.0,
        0.01,
        0.123,
        1.0,
        4.0,
        7.0,
        42.0,
        69.0,
        420.0,
        99999999.0,
      };
    for(auto v : values)
      {
        node.PushFormInput(ind++, nullptr, v);
        auto expecting = node.GetValue() + v;
        auto result = node.GetLastFrame();
        testComment << "expecting=" << expecting << ", result=" << result << "\n";
        testAssert(std::to_string(node.GetValue()) + " + " + std::to_string(v) + " = " + std::to_string(expecting), result == expecting);
      }
  }
}
