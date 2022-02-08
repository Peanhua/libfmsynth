/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeConstant.hh"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace fmsynth;


NodeConstant::NodeConstant()
  : Node("Constant"),
    _constant(1),
    _is_frequency(true)
{
}


double NodeConstant::GetConstant() const
{
  return _constant;
}


void NodeConstant::SetConstant(double constant)
{
  _constant = constant;
}


bool NodeConstant::IsFrequency() const
{
  return _is_frequency;
}


void NodeConstant::SetIsFrequency(bool is_frequency)
{
  _is_frequency = is_frequency;
}


double NodeConstant::ProcessInput([[maybe_unused]] double time, [[maybe_unused]] double form)
{
  auto m = _constant;
  if(_is_frequency)
    m *= std::numbers::pi * 2.0;

  return m;
}


Input::Range NodeConstant::GetFormOutputRange() const
{
  auto c = _constant;
  if(_is_frequency)
    c *= std::numbers::pi * 2.0;

  if(c >= 0 && c <= 1)
    return Input::Range::Zero_One;
  else if(c >= -1 && c <= 1)
    return Input::Range::MinusOne_One;
  else
    return Input::Range::Inf_Inf;
}


json11::Json NodeConstant::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["constant_value"] = _constant;
  rv["is_frequency"]   = _is_frequency;
  return rv;
}


void NodeConstant::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  assert(json["constant_value"].is_number());
  _constant = json["constant_value"].number_value();
  if(json["is_frequency"].is_bool())
    _is_frequency = json["is_frequency"].bool_value();
}
