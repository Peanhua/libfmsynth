/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeTimeScale.hh"

using namespace fmsynth;

NodeTimeScale::NodeTimeScale()
  : Node("TimeScale"),
    _scale(1)
{
}


double NodeTimeScale::GetScale() const
{
  return _scale;
}


void NodeTimeScale::SetScale(double scale)
{
  _scale = scale;
}


void NodeTimeScale::ResetTime()
{
  Node::ResetTime();
  _samplebuffer.clear();
  _timebuffer.clear();
}


double NodeTimeScale::ProcessInput([[maybe_unused]] double time, double form)
{
  if(_scale > 1.0)
    { // Shrink the input to shorter output.
      assert(false); // It is not possible to see into the future now because the input node is not controlled by us.
      return form;
    }
  else if(_scale < 1.0)
    { // Extend the input to longer output.
      _samplebuffer.push_back(form);
      _timebuffer.push_back(time);

      if(time <= 0.0)
        return _samplebuffer[0];
      
      double current_time = time * _scale;
      assert(current_time > _timebuffer[0]);
      double time_per_sample = 1.0 / GetSamplesPerSecond();
      double alpha = (current_time - _timebuffer[0]) / time_per_sample;
      if(alpha > 1.0)
        {
          alpha -= 1.0;
          _samplebuffer.pop_front();
          _timebuffer.pop_front();
        }
      return (1.0-alpha)*_samplebuffer[0] + alpha*_samplebuffer[1];
    }
  else
    { // No-op.
      return form;
    }
}


Input::Range NodeTimeScale::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}


json11::Json NodeTimeScale::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["scale"] = _scale;
  return rv;
}


void NodeTimeScale::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _scale = json["scale"].number_value();
}
