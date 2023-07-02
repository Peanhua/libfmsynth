/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeRangeConvert.hh"
#include <cassert>

using namespace fmsynth;


NodeRangeConvert::NodeRangeConvert()
  : Node("RangeConvert"),
    _from(0, 1),
    _to(0, 1)
{
}


const Range & NodeRangeConvert::GetFrom() const
{
  return _from;
}


const Range & NodeRangeConvert::GetTo() const
{
  return _to;
}


void NodeRangeConvert::SetFrom(const Range & range)
{
  _from = range;
}


void NodeRangeConvert::SetTo(const Range & range)
{
  _to = range;
}


double NodeRangeConvert::ProcessInput([[maybe_unused]] double time, double form)
{
  return _from.ConvertTo(form, _to);
}




Range::Range(double min, double max)
  : _min(min),
    _max(max)
{
  assert(_min < _max);
}

double Range::ConvertTo(double value, const Range & to)
{
  value -= _min;
  value /= (_max - _min);
  return to._min + value * (to._max - to._min);
}

double Range::GetMin() const
{
  return _min;
}

double Range::GetMax() const
{
  return _max;
}

void Range::Set(double min, double max)
{
  _min = min;
  _max = max;
}


Input::Range NodeRangeConvert::GetFormOutputRange() const
{
  auto min = _to.GetMin();
  auto max = _to.GetMax();

  if(min >= 0 && max <= 1)
    return Input::Range::Zero_One;
  else if(min >= -1 && max <= 1)
    return Input::Range::MinusOne_One;
  else
    return Input::Range::Inf_Inf;
}


json11::Json NodeRangeConvert::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["range_convert_custom_from"] = json11::Json::array { _from.GetMin(), _from.GetMax() };
  rv["range_convert_custom_to"]   = json11::Json::array { _to.GetMin(),   _to.GetMax()   };
  return rv;
}

void NodeRangeConvert::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);

  auto & cfrom = json["range_convert_custom_from"];
  _from.Set(cfrom[0].number_value(), cfrom[1].number_value());

  auto & cto = json["range_convert_custom_to"];
  _to.Set(cto[0].number_value(), cto[1].number_value());
}
