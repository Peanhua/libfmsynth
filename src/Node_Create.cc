/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

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
#include "NodeSmooth.hh"
#include "NodeTimeScale.hh"
#include <cassert>

using namespace fmsynth;


std::shared_ptr<Node> Node::Create(const json11::Json & json)
{
  assert(json["node_type"].is_string());
  auto type = json["node_type"].string_value();

  std::shared_ptr<Node> node;
  if(type == "Constant")
    node = std::make_shared<NodeConstant>();
  else if(type == "Growth")
    node = std::make_shared<NodeGrowth>();
  else if(type == "FileOutput")
    node = std::make_shared<NodeFileOutput>();
  else if(type == "AudioDeviceOutput")
    node = std::make_shared<NodeAudioDeviceOutput>();
  else if(type == "Oscillator" || type == "Sine" || type == "Pulse" || type == "Triangle" || type == "Sawtooth" || type == "Noise")
    node = std::make_shared<NodeOscillator>();
  else if(type == "ADHSR")
    node = std::make_shared<NodeADHSR>();
  else if(type == "Add")
    node = std::make_shared<NodeAdd>();
  else if(type == "Multiply")
    node = std::make_shared<NodeMultiply>();
  else if(type == "Average")
    node = std::make_shared<NodeAverage>();
  else if(type == "Filter" || type == "LowPass" || type == "HighPass")
    node = std::make_shared<NodeFilter>();
  else if(type == "RangeConvert")
    node = std::make_shared<NodeRangeConvert>();
  else if(type == "Clamp")
    node = std::make_shared<NodeClamp>();
  else if(type == "Reciprocal")
    node = std::make_shared<NodeReciprocal>();
  else if(type == "Delay")
    node = std::make_shared<NodeDelay>();
  else if(type == "Inverse")
    node = std::make_shared<NodeInverse>();
  else if(type == "Smooth")
    node = std::make_shared<NodeSmooth>();
  else if(type == "TimeScale")
    node = std::make_shared<NodeTimeScale>();
  else if(type == "Comment" || type == "ViewWaveform")
    ;
  else
    assert(false);

  if(node)
    node->SetFromJson(json);

  return node;
}
