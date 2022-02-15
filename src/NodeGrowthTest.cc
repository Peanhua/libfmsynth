/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Blueprint.hh"
#include "NodeGrowth.hh"
#include "Test.hh"


static int Test()
{
  int testid = 1;
  {
    fmsynth::Blueprint bp;
    bp.SetSamplesPerSecond(10);
    
    fmsynth::NodeGrowth node;
    node.ParamStartValue()    = { 0, fmsynth::ConstantValue::Unit::Absolute };
    node.ParamGrowthFormula() = fmsynth::NodeGrowth::Formula::Linear;
    node.ParamGrowthAmount()  = { 1, fmsynth::ConstantValue::Unit::Absolute };
    node.ParamEndAction()     = fmsynth::NodeGrowth::EndAction::NoEnd;
    bp.AddNode(&node);

    bool success = true;
    for(int i = 0; success && i < 100; i++)
      {
        bp.Tick(1);
        success = std::abs(node.GetLastFrame() - static_cast<double>(i) / 10.0) < 0.1;
        std::cout << "# i=" << i << ", NodeGrowth.GetLastFrame()=" << node.GetLastFrame() << " : " << success << "\n";
      }
    testAssert(testid++, "Linear-NoEnd grows expectedly.", success);
  }

  {
    fmsynth::Blueprint bp;
    bp.SetSamplesPerSecond(10);
    
    fmsynth::NodeGrowth node;
    node.ParamStartValue()    = { 0, fmsynth::ConstantValue::Unit::Absolute };
    node.ParamGrowthFormula() = fmsynth::NodeGrowth::Formula::Linear;
    node.ParamGrowthAmount()  = { 1, fmsynth::ConstantValue::Unit::Absolute };
    node.ParamEndAction()     = fmsynth::NodeGrowth::EndAction::RepeatLast;
    node.ParamEndValue()      = { 1, fmsynth::ConstantValue::Unit::Absolute };
    bp.AddNode(&node);

    bool success = true;
    for(int i = 0; success && i < 10; i++)
      {
        bp.Tick(1);
        success = std::abs(node.GetLastFrame() - static_cast<double>(i) / 10.0) < 0.1;
        std::cout << "# i=" << i << ", NodeGrowth.GetLastFrame()=" << node.GetLastFrame() << " : " << success << "\n";
      }
    testAssert(testid++, "Linear-RepeatLast grows expectedly.", success);

    for(int i = 0; success && i < 100; i++)
      {
        bp.Tick(1);
        success = std::abs(node.GetLastFrame() - 1.0) < 0.1;
        std::cout << "# i=" << (10 + i) << ", NodeGrowth.GetLastFrame()=" << node.GetLastFrame() << " : " << success << "\n";
      }
    testAssert(testid++, "Linear-RepeatLast stays same after end.", success);
  }
  

  return testid;
}
