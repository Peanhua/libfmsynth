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
#include "ConstantValue.hh"
#include "NodeOscillator.hh"

static void Test()
{
  {
    const unsigned int sps = 50000;
    fmsynth::ConstantValue hz { 1, fmsynth::ConstantValue::Unit::Hertz };

    fmsynth::NodeOscillator o;
    o.SetSamplesPerSecond(sps);
    o.SetType(fmsynth::NodeOscillator::Type::SINE);
    o.AddFormInputNode(nullptr);

    unsigned int ind = 0;
    o.PushFormInput(ind++, nullptr, hz.GetValue());
    auto first = o.GetLastFrame();
    auto prevval = first;
    unsigned int changes = 0;
    for(unsigned int i = 0; i < sps; i++)
      {
        o.PushFormInput(ind++, nullptr, hz.GetValue());

        auto v = o.GetLastFrame();
        if(v != prevval)
          changes++;
        prevval = v;
      }
    testComment << "hz=" << hz.GetValue() << ", first=" << first << ", last=" << o.GetLastFrame() << ", changes=" << changes << "\n";
    
    testAssert("Oscillator node returns nearly the same value after 1 second with 1Hz sine wave.", std::abs(first - o.GetLastFrame()) < 0.0001);
    testAssert("Oscillator node returns different values for most timesteps.", changes > sps / 2);
  }
}
