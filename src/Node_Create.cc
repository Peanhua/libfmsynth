/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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
#include "NodeInverse.hh"
#include "NodeMultiply.hh"
#include "NodeOscillator.hh"
#include "NodeRangeConvert.hh"
#include "NodeReciprocal.hh"
#include <cassert>


Node * Node::Create(const json11::Json & json)
{
  assert(json["node_type"].is_string());
  auto type = json["node_type"].string_value();

  Node * node = nullptr;
  if(type == "Constant")
    node = new NodeConstant();
  else if(type == "FileOutput")
    node = new NodeFileOutput();
  else if(type == "AudioDeviceOutput")
    node = new NodeAudioDeviceOutput();
  else if(type == "Oscillator" || type == "Sine" || type == "Pulse" || type == "Triangle" || type == "Sawtooth" || type == "Noise")
    node = new NodeOscillator();
  else if(type == "ADHSR")
    node = new NodeADHSR();
  else if(type == "Add")
    node = new NodeAdd();
  else if(type == "Multiply")
    node = new NodeMultiply();
  else if(type == "Average")
    node = new NodeAverage();
  else if(type == "Filter" || type == "LowPass" || type == "HighPass")
    node = new NodeFilter();
  else if(type == "RangeConvert")
    node = new NodeRangeConvert();
  else if(type == "Clamp")
    node = new NodeClamp();
  else if(type == "Reciprocal")
    node = new NodeReciprocal();
  else if(type == "Delay")
    node = new NodeDelay();
  else if(type == "Inverse")
    node = new NodeInverse();
  else if(type == "Comment" || type == "ViewWaveform")
    ;
  else
    assert(false);

  if(node)
    node->SetFromJson(json);

  return node;
}
