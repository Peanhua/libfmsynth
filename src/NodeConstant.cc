/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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
  : Node("Constant")
{
}


ConstantValue & NodeConstant::GetValue()
{
  return _value;
}


const ConstantValue & NodeConstant::GetValue() const
{
  return _value;
}


double NodeConstant::ProcessInput([[maybe_unused]] double time, [[maybe_unused]] double form)
{
  return _value.GetValue();
}


Input::Range NodeConstant::GetFormOutputRange() const
{
  auto c = _value.GetValue();

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
  rv["constant"] = _value.to_json();
  return rv;
}


void NodeConstant::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  if(json["constant"].is_object())
    _value.SetFromJson(json["constant"]);
  else
    { // v0.1 file format read support:
      assert(json["constant_value"].is_number());
      auto unit = fmsynth::ConstantValue::Unit::Absolute;
      if(json["is_frequency"].is_bool() && json["is_frequency"].bool_value())
        unit = fmsynth::ConstantValue::Unit::Hertz;

      _value = fmsynth::ConstantValue {
        json["constant_value"].number_value(),
        unit
      };
    }
}
