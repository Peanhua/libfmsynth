/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Blueprint.hh"
#include "NodeGrowth.hh"
#include "Test.hh"


static void Test()
{
  {
    fmsynth::Blueprint bp;
    bp.SetSamplesPerSecond(10);
    
    auto node = std::make_shared<fmsynth::NodeGrowth>();
    node->ParamStartValue()    = { 0, fmsynth::ConstantValue::Unit::Absolute };
    node->ParamGrowthFormula() = fmsynth::NodeGrowth::Formula::Linear;
    node->ParamGrowthAmount()  = { 1, fmsynth::ConstantValue::Unit::Absolute };
    node->ParamEndAction()     = fmsynth::NodeGrowth::EndAction::NoEnd;
    bp.AddNode(node);

    std::string test_name = "Linear-NoEnd grows expectedly.";
#if LIBFMSYNTH_ENABLE_NODETESTING
    bool success = true;
    for(int i = 0; success && i < 100; i++)
      {
        bp.Tick(1);
        success = std::abs(node->GetLastFrame() - static_cast<double>(i) / 10.0) < 0.1;
        testComment << "i=" << i << ", NodeGrowth.GetLastFrame()=" << node->GetLastFrame() << " : " << success << "\n";
      }
    testAssert(test_name, success);
#else
    testSkip(test_name, "NodeTesting is disabled.");
#endif
  }

  {
    fmsynth::Blueprint bp;
    bp.SetSamplesPerSecond(10);
    
    auto node = std::make_shared<fmsynth::NodeGrowth>();
    node->ParamStartValue()    = { 0, fmsynth::ConstantValue::Unit::Absolute };
    node->ParamGrowthFormula() = fmsynth::NodeGrowth::Formula::Linear;
    node->ParamGrowthAmount()  = { 1, fmsynth::ConstantValue::Unit::Absolute };
    node->ParamEndAction()     = fmsynth::NodeGrowth::EndAction::RepeatLast;
    node->ParamEndValue()      = { 1, fmsynth::ConstantValue::Unit::Absolute };
    bp.AddNode(node);

    std::string test_name = "Linear-RepeatLast grows expectedly.";
#if LIBFMSYNTH_ENABLE_NODETESTING
    bool success = true;
    for(int i = 0; success && i < 10; i++)
      {
        bp.Tick(1);
        success = std::abs(node->GetLastFrame() - static_cast<double>(i) / 10.0) < 0.1;
        testComment << "i=" << i << ", NodeGrowth.GetLastFrame()=" << node->GetLastFrame() << " : " << success << "\n";
      }
    testAssert(test_name, success);
#else
    testSkip(test_name, "NodeTesting is disabled.");
#endif

    test_name = "Linear-RepeatLast stays same after end.";
#if LIBFMSYNTH_ENABLE_NODETESTING
    success = true;
    for(int i = 0; success && i < 100; i++)
      {
        bp.Tick(1);
        success = std::abs(node->GetLastFrame() - 1.0) < 0.1;
        testComment << "i=" << (10 + i) << ", NodeGrowth.GetLastFrame()=" << node->GetLastFrame() << " : " << success << "\n";
      }
    testAssert(test_name, success);
#else
    testSkip(test_name, "NodeTesting is disabled.");
#endif
  }
}
