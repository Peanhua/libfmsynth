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
      node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
      node.SetFrom(r.first);
      node.SetTo(r.second);

      long ind = 0;

      auto input = r.first.GetMin();
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      auto expecting = r.second.GetMin();
      std::string test_name =
        "From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
        " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
        " with input " + std::to_string(input) +
        " result is correct (" + std::to_string(expecting) + ")";
#if LIBFMSYNTH_ENABLE_NODETESTING
      auto result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert(test_name, result == expecting);
#else
      testSkip(test_name, "NodeTesting is disabled.");
#endif

      input = r.first.GetMax();
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      expecting = r.second.GetMax();
      test_name =
        "From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
        " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
        " with input " + std::to_string(input) +
        " result is correct (" + std::to_string(expecting) + ")";
#if LIBFMSYNTH_ENABLE_NODETESTING
      result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert(test_name, result == expecting);
#else
      testSkip(test_name, "NodeTesting is disabled.");
#endif

      input = (r.first.GetMin() + r.first.GetMax()) / 2.0;
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, input);
      node.FinishFrame(ind++);
      expecting = (r.second.GetMin() + r.second.GetMax()) / 2.0;
      test_name =
        "From [" + std::to_string(r.first.GetMin()) + "," + std::to_string(r.first.GetMax()) + "]" +
        " to [" + std::to_string(r.second.GetMin()) + "," + std::to_string(r.second.GetMax()) + "]" +
        " with input " + std::to_string(input) +
        " result is correct (" + std::to_string(expecting) + ")";
#if LIBFMSYNTH_ENABLE_NODETESTING
      result = node.GetLastFrame();
      testComment << "input=" << input << ", expecting=" << expecting << ", result=" << result << "\n";
      testAssert(test_name, result == expecting);
#else
      testSkip(test_name, "NodeTesting is disabled.");
#endif
    }
}
