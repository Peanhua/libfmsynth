/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeGrowth.hh"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace fmsynth;


NodeGrowth::NodeGrowth()
  : Node("Growth"),
    _start_value(0, ConstantValue::Unit::Absolute),
    _growth_formula(Formula::Linear),
    _growth_amount(1, ConstantValue::Unit::Absolute),
    _end_action(EndAction::NoEnd),
    _end_value(0, ConstantValue::Unit::Absolute),
    _start_time(0),
    _current_value(_start_value.GetValue())
{
}


double NodeGrowth::ProcessInput(double time, [[maybe_unused]] double form)
{
  auto GetNextValue = [this, &time]() -> double
  {
    auto t = time - _start_time;
    switch(_growth_formula)
      {
      case Formula::Linear:
        return _start_value.GetValue() + t * _growth_amount.GetValue();
      case Formula::Logistic:
        return _start_value.GetValue() + (0.5 + 0.5 * std::tanh(t / 2.0)) * (_growth_amount.GetValue() - _start_value.GetValue());
      case Formula::Exponential:
        return _start_value.GetValue() + t * std::pow(2.0, _growth_amount.GetValue());
      }
    assert(false);
    return 0;
  };

  auto IsEnded = [this]() -> bool
  {
    if(_growth_amount.GetValue() < 0)
      {
        if(_current_value <= _end_value.GetValue())
          return true;
      }
    else
      {
        if(_current_value >= _end_value.GetValue())
          return true;
      }
    return false;
  };

  if(!IsEnded())
    _current_value = GetNextValue();
  else
    switch(_end_action)
      {
      case EndAction::NoEnd:
        _current_value = GetNextValue();
        break;
      case EndAction::RepeatLast:
        break;
      case EndAction::RestartFromStart:
        _start_time = time;
        _current_value = _start_value.GetValue();
        break;
      case EndAction::Stop:
        SetIsFinished();
        break;
      }
  
  return _current_value;
}


void NodeGrowth::ResetTime()
{
  Node::ResetTime();
  _start_time = 0;
  _current_value = _start_value.GetValue();
}


Input::Range NodeGrowth::GetFormOutputRange() const
{
  return Input::Range::Inf_Inf;
}


json11::Json NodeGrowth::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["growth_start_value"] = _start_value.to_json();
  rv["growth_formula"]     = static_cast<int>(_growth_formula);
  rv["growth_amount"]      = _growth_amount.to_json();
  rv["growth_end_action"]  = static_cast<int>(_end_action);
  rv["growth_end_value"]   = _end_value.to_json();
  return rv;
}


void NodeGrowth::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _start_value.SetFromJson(json["growth_start_value"]);
  _growth_formula = static_cast<Formula>(json["growth_formula"].int_value());
  _growth_amount.SetFromJson(json["growth_amount"]);
  _end_action = static_cast<EndAction>(json["growth_end_action"].int_value());
  _end_value.SetFromJson(json["growth_end_value"]);
}
