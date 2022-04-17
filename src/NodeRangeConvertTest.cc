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
#include "NodeRangeConvert.hh"

static void Test()
{
  std::vector<std::pair<fmsynth::Range, fmsynth::Range>> ranges
    {
      { {  0, 1 }, {  0, 1 } },
      { {  0, 1 }, { -1, 1 } },
      { { -1, 1 }, {  0, 1 } },
      { { -1, 1 }, { -1, 1 } },
      { {  0, 1 }, {  0, 2 } },
      { {  0, 1 }, { -1, 2 } },
      { { -1, 1 }, {  0, 2 } },
      { { -1, 1 }, { -1, 2 } },
      { {  0, 2 }, {  0, 1 } },
      { {  0, 2 }, { -1, 1 } },
      { { -1, 2 }, {  0, 1 } },
      { { -1, 2 }, { -1, 1 } },
      { { -1, 1 }, { -2, 2 } },
      { { -2, 2 }, { -1, 1 } },
    };
  for(auto r : ranges)
    {
      fmsynth::NodeRangeConvert node;
      node.SetSamplesPerSecond(30000);
      fmsynth::Node::Connect(fmsynth::Node::Channel::Form, nullptr, fmsynth::Node::Channel::Form, &node);
      node.SetFrom(r.first);
      node.SetTo(r.second);

      unsigned int ind = 0;

      auto input = r.first.GetMin();
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      auto expecting = r.second.GetMin();
      auto result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert("From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
                 " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
                 " with input " + std::to_string(input) +
                 " result is correct.", result == expecting);

      input = r.first.GetMax();
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      expecting = r.second.GetMax();
      result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert("From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
                 " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
                 " with input " + std::to_string(input) +
                 " result is correct.", result == expecting);

      input = (r.first.GetMin() + r.first.GetMax()) / 2.0;
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      expecting = (r.second.GetMin() + r.second.GetMax()) / 2.0;
      result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert("From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
                 " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
                 " with input " + std::to_string(input) +
                 " result is correct.", result == expecting);
    }
}
