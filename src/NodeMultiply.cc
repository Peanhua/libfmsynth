/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeMultiply.hh"


NodeMultiply::NodeMultiply()
  : Node("Multiply"),
    _multiplier(1)
{
  GetFormInput()->SetDefaultValue(1);
}


double NodeMultiply::GetMultiplier() const
{
  return _multiplier;
}


void NodeMultiply::SetMultiplier(double multiplier)
{
  _multiplier = multiplier;
}


double NodeMultiply::ProcessInput([[maybe_unused]] double time, double form)
{
  return form * _multiplier;
}


Input::Range NodeMultiply::GetAmplitudeInputRange() const
{
  return GetAmplitudeInput()->GetInputRange();
}


Input::Range NodeMultiply::GetFormOutputRange() const
{
  return GetAmplitudeInput()->GetInputRange();
}


json11::Json NodeMultiply::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["multiply_value"] = _multiplier;
  return rv;
}

void NodeMultiply::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _multiplier = json["multiply_value"].number_value();
}
