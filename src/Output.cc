/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Output.hh"
#include <algorithm>
#include <cassert>

using namespace fmsynth;


void Output::AddOutputNode(Node * node)
{
  assert(node);
  _output_nodes.push_back(node);
}


void Output::RemoveOutputNode(Node * node)
{
  auto it = std::find(_output_nodes.cbegin(), _output_nodes.cend(), node);
  if(it != _output_nodes.cend())
    _output_nodes.erase(it);
}


const std::vector<Node *> & Output::GetOutputNodes() const
{
  return _output_nodes;
}
