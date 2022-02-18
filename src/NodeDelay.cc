/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeDelay.hh"

using namespace fmsynth;


NodeDelay::NodeDelay()
  : Node("Delay"),
    _delay_time(0)
{ // todo: Use std::vector instead.
}


double NodeDelay::GetDelayTime() const
{
  return _delay_time;
}


void NodeDelay::SetDelayTime(double time)
{
  _delay_time = time;
}


void NodeDelay::PrefillBuffer()
{
  _buffer.clear();
  auto n = static_cast<unsigned int>(_delay_time * static_cast<double>(GetSamplesPerSecond()));
  for(unsigned int i = 0; i < n; i++)
    _buffer.push_back(0);
}


double NodeDelay::ProcessInput([[maybe_unused]] double time, double form)
{
  if(_buffer.empty() && _delay_time > 0.0)
    PrefillBuffer();
  
  _buffer.push_back(form);
  auto rv = _buffer.front();
  _buffer.pop_front();
  return rv;
}


void NodeDelay::ResetTime()
{
  Node::ResetTime();
  PrefillBuffer();
}


Input::Range NodeDelay::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}


json11::Json NodeDelay::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["delay_time"] = _delay_time;
  return rv;
}

void NodeDelay::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _delay_time = json["delay_time"].number_value();
  PrefillBuffer();
}
