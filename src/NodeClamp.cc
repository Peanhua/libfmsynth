/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeClamp.hh"
#include <algorithm>

using namespace fmsynth;


NodeClamp::NodeClamp()
  : Node("Clamp"),
    _min(0),
    _max(1)
{
}


double NodeClamp::GetMin() const
{
  return _min;
}


double NodeClamp::GetMax() const
{
  return _max;
}


void NodeClamp::SetMin(double min)
{
  _min = min;
}


void NodeClamp::SetMax(double max)
{
  _max = max;
}


double NodeClamp::ProcessInput([[maybe_unused]] double time, double form)
{
  return std::clamp(form, _min, _max);
}


json11::Json NodeClamp::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["clamp_min"] = _min;
  rv["clamp_max"] = _max;
  return rv;
}

void NodeClamp::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _min = json["clamp_min"].number_value();
  _max = json["clamp_max"].number_value();
}


Input::Range NodeClamp::GetFormOutputRange() const
{
  if(_min >= 0 && _max <= 1)
    return Input::Range::Zero_One;
  else if(_min >= -1 && _max <= 1)
    return Input::Range::MinusOne_One;
  else
    return Input::Range::Inf_Inf;
}
