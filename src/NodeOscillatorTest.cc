/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Test.hh"
#include "ConstantValue.hh"
#include "NodeOscillator.hh"

static void Test()
{
#if LIBFMSYNTH_ENABLE_NODETESTING
  {
    const unsigned int sps = 50000;
    fmsynth::ConstantValue hz { 1, fmsynth::ConstantValue::Unit::Hertz };

    fmsynth::NodeOscillator o;
    o.SetSamplesPerSecond(sps);
    o.SetType(fmsynth::NodeOscillator::Type::SINE);
    o.AddInputNode(fmsynth::Node::Channel::Form, nullptr);

    long ind = 0;
    o.PushInput(nullptr, fmsynth::Node::Channel::Form, hz.GetValue());
    o.FinishFrame(ind++);
    auto first = o.GetLastFrame();
    auto prevval = first;
    unsigned int changes = 0;
    for(unsigned int i = 0; i < sps; i++)
      {
        o.PushInput(nullptr, fmsynth::Node::Channel::Form, hz.GetValue());
        o.FinishFrame(ind++);

        auto v = o.GetLastFrame();
        if(!FloatEqual(v, prevval, 0.00001))
          changes++;
        prevval = v;
      }
    testComment << "hz=" << hz.GetValue() << ", first=" << first << ", last=" << o.GetLastFrame() << ", changes=" << changes << "\n";
    
    testAssert("Oscillator node returns nearly the same value after 1 second with 1Hz sine wave.", std::abs(first - o.GetLastFrame()) < 0.0001);
    testAssert("Oscillator node returns different values for most timesteps.", changes > sps / 2);
  }
#else
  testSkip("Oscillator node returns nearly the same value after 1 second with 1Hz sine wave.", "NodeTesting is disabled.");
  testSkip("Oscillator node returns different values for most timesteps.", "NodeTesting is disabled.");
#endif
}
