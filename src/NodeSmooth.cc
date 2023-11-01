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


unsigned int NodeSmooth::GetWindowSize() const
{
  return static_cast<unsigned int>(_window.size());
}


void NodeSmooth::SetWindowSize(unsigned int size)
{
  _window.resize(size);
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
  if(_position != _datasize)
    _lastsum -= _window[_position];
  // Add the new data to sum:
  _lastsum += form;
  
  // Append the data:
  _window[_position] = form;
  _position++;
  if(_position >= _window.size())
    _position = 0;
  if(_datasize < _window.size())
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
