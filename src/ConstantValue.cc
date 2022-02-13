/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "ConstantValue.hh"
#include <cassert>

using namespace fmsynth;


ConstantValue::ConstantValue()
  : ConstantValue(1, Unit::Absolute)
{
}


ConstantValue::ConstantValue(double value, Unit unit)
  : _value(value),
    _unit(unit)
{
}


double ConstantValue::GetValue() const
{
  switch(_unit)
    {
    case Unit::Absolute: return _value;
    case Unit::Hertz:    return _value * std::numbers::pi * 2.0;
    case Unit::Pi:       return _value * std::numbers::pi;
    case Unit::E:        return _value * std::numbers::e;
    }
  assert(false);
  return _value;
}


double ConstantValue::GetUnitValue() const
{
  return _value;
}


ConstantValue::Unit ConstantValue::GetUnit() const
{
  return _unit;
}


json11::Json ConstantValue::to_json() const
{
  return json11::Json::object {
    { "value", _value                   },
    { "unit",  static_cast<int>(_unit)  }
  };
}


void ConstantValue::SetFromJson(const json11::Json & json)
{
  assert(json["value"].is_number());
  _value = json["value"].number_value();

  assert(json["unit"].is_number());
  _unit = static_cast<Unit>(json["unit"].int_value()); // todo: Add checking for validity.
}
