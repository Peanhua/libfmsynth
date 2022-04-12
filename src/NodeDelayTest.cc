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
#include "NodeDelay.hh"

static void Test()
{
  {
    const unsigned int delay = 100;
    const unsigned int sps = 30000;
    
    fmsynth::NodeDelay node;
    node.SetSamplesPerSecond(sps);
    node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
    node.SetDelayTime(delay);

    double v = 123.456;
    unsigned int ind = 0;
    double result = 0;
    for(unsigned int i = 0; i < delay * sps; i++)
      {
        node.PushInput(nullptr, fmsynth::Node::Channel::Form, v);
        node.FinishFrame(ind++);
        result += node.GetLastFrame();
      }
    testComment << "result=" << result << "\n";
    testAssert("First " + std::to_string(delay) + "s of output is silence.", result < 0.0001);

    node.PushInput(nullptr, fmsynth::Node::Channel::Form, v);
    node.FinishFrame(ind++);
    
    testComment << "value input = " << v << ", value after silence = " << node.GetLastFrame() << "\n";
    testAssert("The value input is returned after silence.", std::abs(node.GetLastFrame() - v) < 0.0001);
  }
}
