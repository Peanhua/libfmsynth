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
#include "NodeSmooth.hh"
#include "StdFormat.hh"


static void Test()
{
#if LIBFMSYNTH_ENABLE_NODETESTING
  {
    const unsigned int sps = 50000;

    {
      fmsynth::NodeSmooth node;
      node.SetSamplesPerSecond(sps);
      node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
      node.SetWindowSize(10);

      long ind = 0;
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, 1);
      node.FinishFrame(ind++);
      node.PushInput(nullptr, fmsynth::Node::Channel::Form, 3);
      node.FinishFrame(ind++);
      testAssert("Smooth node returns correct average for values 1 and 3.", FloatEqual(node.GetLastFrame(), 2, 0.00001));
    }
    {
      fmsynth::NodeSmooth node;
      node.SetSamplesPerSecond(sps);
      node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
      node.SetWindowSize(10);
      
      double v = 0.346135;
      long ind = 0;
      for(int i = 0; i < 10; i++)
        {
          node.PushInput(nullptr, fmsynth::Node::Channel::Form, v);
          node.FinishFrame(ind++);
          double result = node.GetLastFrame();
          testComment << "input=" << v << ", output=" << result << "\n";
          testAssert(format("Smooth node with window size {} returns the input value when same value is inserted {} times.", node.GetWindowSize(), i+1), FloatEqual(result, v, 0.00001));
        }
    }
    {
      fmsynth::NodeSmooth node;
      node.SetSamplesPerSecond(sps);
      node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
      node.SetWindowSize(10);
      
      double v = 0.346135;
      long ind = 0;
      for(int i = 0; i < 1000000; i++)
        {
          node.PushInput(nullptr, fmsynth::Node::Channel::Form, v);
          node.FinishFrame(ind++);
        }
      double result = node.GetLastFrame();
      testComment << "input=" << v << ", output=" << result << "\n";
      testAssert("Smooth node does not choke when inputting more values than the size of the window.", FloatEqual(result, v, 0.00001));
    }
    {
      fmsynth::NodeSmooth node;
      node.SetSamplesPerSecond(sps);
      node.AddInputNode(fmsynth::Node::Channel::Form, nullptr);
      node.SetWindowSize(10);

      long ind = 0;
      for(int i = 1; i <= 10; i++)
        {
          node.PushInput(nullptr, fmsynth::Node::Channel::Form, static_cast<double>(i));
          node.FinishFrame(ind++);
        }
      testAssert("Smooth node returns correct average for 10 values in range [1,10].", FloatEqual(node.GetLastFrame(), 5.5, 0.00001));

      double v = 0.8274;
      for(int i = 0; i < 10; i++)
        {
          node.PushInput(nullptr, fmsynth::Node::Channel::Form, v);
          node.FinishFrame(ind++);
        }
      testAssert("Smooth node returns same value after filling the window with one value.", FloatEqual(node.GetLastFrame(), v, 0.00001));
    }
  }
#else
  testSkip("Smooth node testing.", "NodeTesting is disabled.");
#endif
}
