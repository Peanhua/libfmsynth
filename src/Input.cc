/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Input.hh"
#include "Node.hh"
#include <algorithm>
#include <cassert>

using namespace fmsynth;


void Input::SetInputRange(Range range)
{
  _input_range = range;
}


void Input::SetDefaultValue(double new_default_value)
{
  _default_value = new_default_value;
}


bool Input::IsReady() const
{
  assert(_input_count <= _input_nodes.size());
  return _input_count == _input_nodes.size();
}


double Input::GetValueAndReset()
{
  assert(IsReady());

  auto rv = GetValue();
  Reset();

  return rv;
}

double Input::GetValue() const
{
  assert(IsReady());

  if(_input_nodes.size() > 0)
    return _value;
  else
    return _default_value;
}

void Input::Reset()
{
  assert(IsReady());
  _value = 0;
  _input_count = 0;
}

void Input::AddInputNode(Node * node)
{
  _input_nodes.push_back(node);
}


void Input::RemoveInputNode(Node * node)
{
  auto it = std::find(_input_nodes.cbegin(), _input_nodes.cend(), node);
  assert(it != _input_nodes.cend());
  if(it != _input_nodes.cend())
    _input_nodes.erase(it);
}


void Input::InputAdd(Node * source, double value)
{
  assert(std::find(_input_nodes.cbegin(), _input_nodes.cend(), source) != _input_nodes.cend());
  value = NormalizeInputValue(source, value);
  _value += value;
  _input_count++;
}

void Input::InputMultiply(Node * source, double value)
{
  assert(std::find(_input_nodes.cbegin(), _input_nodes.cend(), source) != _input_nodes.cend());
  value = NormalizeInputValue(source, value);
  if(_input_count == 0)
    _value = value;
  else
    _value *= value;

  _input_count++;
}

const std::vector<Node *> & Input::GetInputNodes() const
{
  return _input_nodes;
}


Input::Range Input::GetInputRange() const
{
  Range range = Range::Zero_One;
  for(auto node : _input_nodes)
    if(node)
      {
        auto r = node->GetFormOutputRange();
        switch(r)
          {
          case Range::Inf_Inf:
            range = r;
            break;
          case Range::MinusOne_One:
            if(range == Range::Zero_One)
              range = r;
            break;
          case Range::Zero_One:
            break;
          }
      }
  return range;
}


double Input::NormalizeInputValue(const Node * source, double value) const
{
  if(!source)
    return value;
  
  switch(source->GetFormOutputRange())
    {
    case Range::Zero_One:
      switch(_input_range)
        {
        case Range::Zero_One:     return value;
        case Range::MinusOne_One: return value * 2.0 - 1.0;
        case Range::Inf_Inf:      return value;
        }
      break;
    case Range::MinusOne_One:
      switch(_input_range)
        {
        case Range::Zero_One:     return value * 0.5 + 0.5;
        case Range::MinusOne_One: return value;
        case Range::Inf_Inf:      return value;
        }
      break;
    case Range::Inf_Inf:
      return value;
    }

  assert(false);
  return value;
}
