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
#include "Input.hh"
#include "Node.hh"


class MockNode : public fmsynth::Node
{
public:
  MockNode()
    : Node("Mock")
  {
  }

  void PublicSetOutputRange(fmsynth::Input::Range range)
  {
    SetOutputRange(range);
  }
  
protected:
  double ProcessInput([[maybe_unused]] double time, double form)
  {
    return form;
  }
};


static std::string RangeToString(fmsynth::Input::Range range)
{
  std::string rv = "UNKNOWN";
  switch(range)
    {
    case fmsynth::Input::Range::Zero_One:     rv = "[0, 1]";      break;
    case fmsynth::Input::Range::MinusOne_One: rv = "[-1, 1]";     break;
    case fmsynth::Input::Range::Inf_Inf:      rv = "[-inf, inf]"; break;
    }
  return rv;
}


static void Test()
{
  {
    fmsynth::Input inp;
    inp.SetDefaultValue(42);
    double v = 3.0;
    inp.AddInputNode(nullptr);
    inp.InputAdd(nullptr, v);
    testAssert("GetValue() returns the value added with InputAdd().", inp.GetValue() == v);
  }
  {
    fmsynth::Input inp;
    inp.SetDefaultValue(42);
    double v = 3.0;
    inp.AddInputNode(nullptr);
    inp.InputAdd(nullptr, v);
    testAssert("GetValue() returns the value added with InputMultiply().", inp.GetValue() == v);
  }
  {
    fmsynth::Input inp;
    inp.SetDefaultValue(42);
    MockNode node;
    double v1 = 3.0;
    double v2 = 4.0;
    inp.AddInputNode(nullptr);
    inp.AddInputNode(&node);
    inp.InputAdd(nullptr, v1);
    inp.InputAdd(&node, v2);
    testAssert("GetValue() returns the two values added with InputAdd().", inp.GetValue() == v1 + v2);
  }
  {
    fmsynth::Input inp;
    inp.SetDefaultValue(42);
    MockNode node;
    double v1 = 3.0;
    double v2 = 4.0;
    inp.AddInputNode(nullptr);
    inp.AddInputNode(&node);
    inp.InputMultiply(nullptr, v1);
    inp.InputMultiply(&node, v2);
    testAssert("GetValue() returns the two values added with InputMultiply().", inp.GetValue() == v1 * v2);
  }
  {
    std::vector<std::tuple<fmsynth::Input::Range, fmsynth::Input::Range, double, double>> values
      {
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::Zero_One,      0,    0   },
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::Zero_One,      0.5,  0.5 },
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::Zero_One,      1,    1   },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::Zero_One,     -1,    0   },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::Zero_One,      0,    0.5 },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::Zero_One,      1,    1   },
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::MinusOne_One,  0,   -1   },
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::MinusOne_One,  0.5,  0   },
        { fmsynth::Input::Range::Zero_One,     fmsynth::Input::Range::MinusOne_One,  1,    1   },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::MinusOne_One, -1,   -1   },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::MinusOne_One,  0,    0   },
        { fmsynth::Input::Range::MinusOne_One, fmsynth::Input::Range::MinusOne_One,  1,    1   },
      };
    for(auto [outrange, inrange, input, expected] : values)
      {
        MockNode node;
        node.PublicSetOutputRange(outrange);
        fmsynth::Input inp;
        inp.SetDefaultValue(42);
        inp.SetInputRange(inrange);
        inp.AddInputNode(&node);
        inp.InputAdd(&node, input);
        testComment << "value=" << input
                    << ", from=" << RangeToString(outrange)
                    << ", to=" << RangeToString(inrange)
                    << ", returned=" << inp.GetValue()
                    << ", expected=" << expected
                    << "\n";
        testAssert("Passing value " + std::to_string(input)
                   + " from node with output range " + RangeToString(outrange)
                   + " to input with input range " + RangeToString(inrange)
                   + " produces " + std::to_string(expected) + ".",
                   inp.GetValue() == expected);
      }
  }
}
