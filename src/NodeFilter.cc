/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeFilter.hh"
#include <cassert>
#include <numbers>

using namespace fmsynth;


NodeFilter::NodeFilter()
  : Node("Filter"),
    _type(Type::LOW_PASS),
    _filter(0.5),
    _first(true),
    _lowpass_previous(0),
    _highpass_previous_input(0),
    _highpass_previous_filtered(0)
{
}


NodeFilter::Type NodeFilter::GetType() const
{
  return _type;
}


double NodeFilter::GetFilterValue() const
{
  return _filter;
}


void NodeFilter::SetType(Type type)
{
  _type = type;
}


void NodeFilter::SetFilterValue(double value)
{
  _filter = value;
}


double NodeFilter::ProcessInput([[maybe_unused]] double time, double form)
{
  auto filter = _filter;
  auto aux = GetInput(Channel::Aux);
  if(aux->GetInputNodes().size() > 0)
    filter = aux->GetValue();

  switch(_type)
    {
    case Type::LOW_PASS:  form = LowPass(filter, form);  break;
    case Type::HIGH_PASS: form = HighPass(filter, form); break;
    }
  return form;
}


double NodeFilter::LowPass(double filter, double input)
{
  double output;
  if(_first)
    {
      _first = false;
      output = filter * input;
    }
  else
    {
      output = _lowpass_previous + filter * (input - _lowpass_previous);
    }
  _lowpass_previous = output;
  return output;
}

double NodeFilter::HighPass(double filter, double input)
{
  double output;
  if(_first)
    {
      _first = false;
      output = input;
    }
  else
    {
      output = filter * (_highpass_previous_filtered + input - _highpass_previous_input);
    }
  _highpass_previous_input    = input;
  _highpass_previous_filtered = output;
  return output;
}


Input::Range NodeFilter::GetInputRange(Channel channel) const
{
  if(channel == Channel::Aux)
    return Input::Range::Zero_One;
  else
    return Node::GetInputRange(channel);
}


Input::Range NodeFilter::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}


json11::Json NodeFilter::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["filter_type"]  = static_cast<int>(_type);
  rv["filter_value"] = _filter;
  return rv;
}


void NodeFilter::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _type   = static_cast<Type>(json["filter_type"].int_value());
  _filter = json["filter_value"].number_value();
}
