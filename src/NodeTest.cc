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
#include "NodeADHSR.hh"
#include "NodeAdd.hh"
#include "NodeAudioDeviceOutput.hh"
#include "NodeAverage.hh"
#include "NodeClamp.hh"
#include "NodeConstant.hh"
#include "NodeDelay.hh"
#include "NodeFileOutput.hh"
#include "NodeFilter.hh"
#include "NodeGrowth.hh"
#include "NodeInverse.hh"
#include "NodeMultiply.hh"
#include "NodeOscillator.hh"
#include "NodeRangeConvert.hh"
#include "NodeReciprocal.hh"
#include <tuple>

static int Test()
{
  std::vector<std::tuple<fmsynth::Node *, std::string>> nodes
    {
      { new fmsynth::NodeADHSR(),             "ADHSR"             },
      { new fmsynth::NodeAdd(),               "Add"               },
      { new fmsynth::NodeAudioDeviceOutput(), "AudioDeviceOutput" },
      { new fmsynth::NodeAverage(),           "Average"           },
      { new fmsynth::NodeClamp(),             "Clamp"             },
      { new fmsynth::NodeConstant(),          "Constant"          },
      { new fmsynth::NodeDelay(),             "Delay"             },
      { new fmsynth::NodeFileOutput(),        "FileOutput"        },
      { new fmsynth::NodeFilter(),            "Filter"            },
      { new fmsynth::NodeGrowth(),            "Growth"            },
      { new fmsynth::NodeInverse(),           "Inverse"           },
      { new fmsynth::NodeMultiply(),          "Multiply"          },
      { new fmsynth::NodeOscillator(),        "Oscillator"        },
      { new fmsynth::NodeRangeConvert(),      "RangeConvert"      },
      { new fmsynth::NodeReciprocal(),        "Reciprocal"        },
    };
  auto root = new fmsynth::NodeConstant();
  int id = 1;

  for(auto [node, type] : nodes)
    {
      testAssert(id++, "Node '" + type + "' was created.",                node);
      
      testAssert(id++, "Node '" + type + "' returns the correct type.",   node->GetType() == type);
      
      testAssert(id++, "Node '" + type + "' is initially not finished.",  node->IsFinished() == false);
      
      node->SetIsFinished();
      testAssert(id++, "Finishing node '" + type + "' sets it finished.", node->IsFinished());
      
      testAssert(id++, "Node '" + type + "' is initially enabled.",       node->IsEnabled());
      node->SetEnabled(root, false);
      
      testAssert(id++, "Disabling node '" + type + "' sets it disabled.", node->IsEnabled() == false);
    }

  return id;
}
