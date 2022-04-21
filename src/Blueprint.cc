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
#include "NodeGrowth.hh"
#include <cassert>
#include <queue>

using namespace fmsynth;


Blueprint::Blueprint()
  : _root(new NodeConstant),
    _time_index(0),
    _samples_per_second(44100)
{
  _root->GetValue() = ConstantValue(1, ConstantValue::Unit::Absolute);
  ConnectNodes(Node::Channel::Form, nullptr, Node::Channel::Form, _root);
  _root->SetSamplesPerSecond(_samples_per_second);
}


Blueprint::~Blueprint()
{
}


void Blueprint::AddNode(Node * node)
{
  if(!node)
    return;
  
  if(dynamic_cast<NodeConstant *>(node) || dynamic_cast<NodeGrowth *>(node))
    ConnectNodes(Node::Channel::Form, _root, Node::Channel::Form, node);
  
  _nodes.push_back(node);
  node->SetSamplesPerSecond(_samples_per_second);
}


void Blueprint::RemoveNode(Node * node)
{
  auto it = std::find(_nodes.cbegin(), _nodes.cend(), node);
  if(it != _nodes.cend())
    _nodes.erase(it);

  ResetExecutionOrder();
}


void Blueprint::ResetTime()
{
  _time_index = 0;
  
  _root->ResetTime();
  for(auto n : _nodes)
    if(n)
      n->ResetTime();
}


void Blueprint::ResetExecutionOrder()
{
  _nodes_sorted = false;
}


Node * Blueprint::GetRoot() const
{
  return _root;
}


Node * Blueprint::GetNode(const std::string & id) const
{
  for(auto n : _nodes)
    if(n && n->GetId() == id)
      return n;
  return nullptr;
}


std::vector<Node *> Blueprint::GetAllNodes() const
{
  std::vector<Node *> nodes;

  for(auto n : _nodes)
    if(n)
      nodes.push_back(n);
  
  return nodes;
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
  SortNodesToExecutionOrder();
  for(int i = 0; !IsFinished() && i < samples; i++)
    {
      _root->PushInput(nullptr, Node::Channel::Form, 1);
      _root->FinishFrame(_time_index);

      for(auto node : _exec_nodes)
        node->FinishFrame(_time_index);
      
      _time_index++;
    }
}


bool Blueprint::Load(const json11::Json & json)
{
  if(json["nodes"].is_array())
    {
      for(auto n : json["nodes"].array_items())
        {
          assert(n["node_type"].is_string());
          auto type = n["node_type"].string_value();
          auto node = Node::Create(n);
          if(node)
            AddNode(node);
        }
    }      

  if(json["links"].is_array())
    {
      auto links = json["links"].array_items();

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
            ConnectNodes(Node::Channel::Form,                                   from_node,
                         Node::StringToChannel(l["to_channel"].string_value()), to_node);
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


void Blueprint::SortNodesToExecutionOrder()
{
  if(_nodes_sorted)
    return;

  _exec_nodes.clear();

  // Topological sort using Kahn's algorithm (https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm).
  std::vector<int> nodes_input_counts; // Index in this vector match with _nodes index.
  for(auto node : _nodes)
    {
      auto c =
        node->GetInput(Node::Channel::Amplitude)->GetInputNodes().size() +
        node->GetInput(Node::Channel::Form)->GetInputNodes().size()      +
        node->GetInput(Node::Channel::Aux)->GetInputNodes().size();
      nodes_input_counts.push_back(static_cast<int>(c));
    }
  
  std::queue<Node *> work;
  
  for(auto node : _root->GetAllOutputNodes())
    work.push(node);

  while(!work.empty())
    {
      auto node = work.front();
      work.pop();

      _exec_nodes.push_back(node);

      for(unsigned int i = 0; i < _nodes.size(); i++)
        {
          auto cur = _nodes[i];
          auto all = node->GetAllOutputNodes();
          auto edge_exists = std::find(all.cbegin(), all.cend(), cur) != all.cend();

          if(edge_exists)
            {
              nodes_input_counts[i]--;
              assert(nodes_input_counts[i] >= 0);

              if(nodes_input_counts[i] == 0)
                work.push(cur);
            }
        }
    }

  _nodes_sorted = true;
}


void Blueprint::ConnectNodes(Node::Channel from_channel, Node * from_node, Node::Channel to_channel, Node * to_node)
{
  assert(from_channel == Node::Channel::Form);
  to_node->AddInputNode(to_channel, from_node);
  if(from_node)
    from_node->AddOutputNode(to_channel, to_node);

  ResetExecutionOrder();
}


void Blueprint::DisconnectNodes(Node::Channel from_channel, Node * from_node, Node::Channel to_channel, Node * to_node)
{
  assert(from_channel == Node::Channel::Form);
  to_node->RemoveInputNode(to_channel, from_node);
  if(from_node)
    from_node->RemoveOutputNode(to_channel, to_node);

  ResetExecutionOrder();
}
