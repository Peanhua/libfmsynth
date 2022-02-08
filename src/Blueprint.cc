/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Blueprint.hh"
#include "NodeConstant.hh"
#include <cassert>

using namespace fmsynth;


Blueprint::Blueprint()
  : _root(new NodeConstant),
    _time_index(0),
    _samples_per_second(44100)
{
  _root->SetConstant(1);
  _root->SetIsFrequency(false);
  _root->AddFormInputNode(nullptr);
  _root->SetSamplesPerSecond(_samples_per_second);
}


Blueprint::~Blueprint()
{
}


void Blueprint::AddNode(Node * node)
{
  if(dynamic_cast<NodeConstant *>(node))
    node->AddFormInputNode(_root);
  _nodes.push_back(node);
  node->SetSamplesPerSecond(_samples_per_second);
}


void Blueprint::RemoveNode(Node * node)
{
  for(auto it = _nodes.begin(); it != _nodes.end(); it++)
    if(*it == node)
      {
        _nodes.erase(it);
        return;
      }
}


void Blueprint::AddLink(Node * from, Node * to, const std::string & channel)
{
  if(channel == "Amplitude")
    to->AddAmplitudeInputNode(from);
  else if(channel == "Form")
    to->AddFormInputNode(from);
  else if(channel == "Aux")
    to->AddAuxInputNode(from);
  else
    assert(false);
}



void Blueprint::ResetTime()
{
  _time_index = 0;
  
  _root->ResetTime();
  for(auto n : _nodes)
    if(n)
      n->ResetTime();
}


Node * Blueprint::GetRoot() const
{
  return _root;
}


std::vector<Node *> Blueprint::GetNodesByType(const std::string & type) const
{
  std::vector<Node *> nodes;

  for(auto n : _nodes)
    if(n && n->GetType() == type)
      nodes.push_back(n);
  
  return nodes;
}


void Blueprint::SetIsFinished()
{
  _root->SetIsFinished();
}


bool Blueprint::IsFinished() const
{
  return _root->IsFinished();
}


std::mutex & Blueprint::GetLockMutex()
{
  return _lock_mutex;
}


void Blueprint::Tick(long samples)
{
  assert(samples > 0);
  for(int i = 0; !IsFinished() && i < samples; i++)
    {
      _root->PushFormInput(_time_index, nullptr, 1);
      _time_index++;
    }
}


bool Blueprint::Load(const json11::Json & json)
{
  if(!json["links"].is_array())
    return false;

  if(!json["nodes"].is_array())
    return false;

  auto links = json["links"].array_items();
  auto nodes = json["nodes"].array_items();

  for(auto n : nodes)
    {
      assert(n["node_type"].is_string());
      auto type = n["node_type"].string_value();
      auto node = Node::Create(n);
      if(node)
        AddNode(node);
    }

  auto FindNodeById = [this](const std::string & node_id) -> Node *
  {
    for(auto n : _nodes)
      if(n)
        if(n->GetId() == node_id)
          return n;
    return nullptr;
  };
  
  for(auto l : links)
    {
      auto from_node = FindNodeById(l["from"].string_value());
      auto to_node   = FindNodeById(l["to"].string_value());
      if(from_node && to_node)
        {
          auto to_channel = l["to_channel"].string_value();
          AddLink(from_node, to_node, to_channel);
        }
    }

  return true;
}



void Blueprint::SetSamplesPerSecond(unsigned int samples_per_second)
{
  _samples_per_second = samples_per_second;

  _root->SetSamplesPerSecond(_samples_per_second);

  for(auto node : _nodes)
    if(node)
      node->SetSamplesPerSecond(samples_per_second);

  ResetTime();
}


unsigned int Blueprint::GetSamplesPerSecond() const
{
  return _samples_per_second;
}

