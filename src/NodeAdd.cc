/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeAdd.hh"

using namespace fmsynth;


NodeAdd::NodeAdd()
  : Node("Add"),
    _value(0)
{
  GetInput(Channel::Form)->SetDefaultValue(1);
}


double NodeAdd::GetValue() const
{
  return _value;
}


void NodeAdd::SetValue(double value)
{
  _value = value;
}


double NodeAdd::ProcessInput([[maybe_unused]] double time, double form)
{
  return form + _value;
}


json11::Json NodeAdd::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["add_value"] = _value;
  return rv;
}

void NodeAdd::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _value = json["add_value"].number_value();
}
