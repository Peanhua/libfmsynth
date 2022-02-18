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

static void Test()
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

  for(auto [node, type] : nodes)
    {
      testAssert("Node '" + type + "' was created.",                node);
      
      testAssert("Node '" + type + "' returns the correct type.",   node->GetType() == type);
      
      testAssert("Node '" + type + "' is initially not finished.",  node->IsFinished() == false);
      
      node->SetIsFinished();
      testAssert("Finishing node '" + type + "' sets it finished.", node->IsFinished());
      
      testAssert("Node '" + type + "' is initially enabled.",       node->IsEnabled());
      node->SetEnabled(root, false);
      
      testAssert("Disabling node '" + type + "' sets it disabled.", node->IsEnabled() == false);
    }

  {
    fmsynth::NodeOscillator node;
    testAssert("New node does not have any output nodes connected to it.", node.GetAllConnectedNodes().empty());
  }
  {
    fmsynth::NodeOscillator o1, o2;
    o2.AddInputNode(fmsynth::Node::Channel::Form, &o1);
    testAssert("After adding input node, the node can be seen in the other node as output node.", o1.GetAllConnectedNodes().contains(&o2));
    o2.RemoveInputNode(fmsynth::Node::Channel::Form, &o1);
    testAssert("After removing input node, the node can no longer be seen in the other node as output node.", !o1.GetAllConnectedNodes().contains(&o2));
  }
}
