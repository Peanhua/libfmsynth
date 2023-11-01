/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeSmooth.hh"

using namespace fmsynth;

NodeSmooth::NodeSmooth()
  : Node("Smooth"),
    _position(0),
    _datasize(0),
    _lastsum(0)
{
}


int NodeSmooth::GetWindowSize() const
{
  return static_cast<int>(_window.size());
}


void NodeSmooth::SetWindowSize(int size)
{
  assert(size > 0);
  _window.resize(static_cast<unsigned int>(size));
}


void NodeSmooth::ResetTime()
{
  Node::ResetTime();
  _position = 0;
  _datasize = 0;
  _lastsum  = 0;
}


double NodeSmooth::ProcessInput([[maybe_unused]] double time, double form)
{
  // Remove the oldest data from the sum:
  if(_datasize == static_cast<int>(_window.size()))
    {
      int oldestpos = _position - _datasize;
      if(oldestpos < 0)
        oldestpos += static_cast<int>(_window.size());
      _lastsum -= _window[static_cast<unsigned int>(oldestpos)];
    }
  // Add the new data to sum:
  _lastsum += form;
  
  // Append the data:
  _window[static_cast<unsigned int>(_position)] = form;
  _position++;
  if(_position >= static_cast<int>(_window.size()))
    _position = 0;
  if(_datasize < static_cast<int>(_window.size()))
    _datasize++;
  
  return _lastsum / static_cast<double>(_datasize);
}


Input::Range NodeSmooth::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}


json11::Json NodeSmooth::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["windowsize"] = static_cast<int>(_window.size());
  return rv;
}


void NodeSmooth::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _window.resize(static_cast<unsigned int>(json["windowsize"].int_value()));
}
