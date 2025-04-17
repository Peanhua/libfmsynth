/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"
#include <cassert>
#include <climits>

using namespace fmsynth;


unsigned long Node::_next_id = 1;


Node::Node(const std::string & type)
  : _type(type),
    _id(std::to_string(_next_id)),
    _preprocess_amplitude(false),
    _enabled(true),
    _samples_per_second(0),
    _finished(false),
    _output_range(Input::Range::Inf_Inf)
#if LIBFMSYNTH_ENABLE_NODETESTING
  , _last_frame(0)
#endif
{
  GetInput(Channel::Amplitude)->SetDefaultValue(1);

  UpdateNextId();
}


Node::~Node()
{
  for(auto channel : AllChannels)
    GetInput(channel)->RemoveAllInputNodes();

  for(auto channel : AllChannels)
    GetOutput(channel)->RemoveAllOutputNodes();
}


const std::string & Node::GetNodeType() const
{
  return _type;
}


const std::string & Node::GetId() const
{
  return _id;
}


void Node::SetId(const std::string & id)
{
  _id = id;
  UpdateNextId();
}


void Node::SetPreprocessAmplitude()
{
  assert(!_preprocess_amplitude);
  _preprocess_amplitude = true;
}


void Node::OnInputConnected(Node * from)
{
  if(from)
    SetSamplesPerSecond(from->GetSamplesPerSecond());
}


void Node::AddInputNode(Channel from_channel, Node * from_node)
{
  GetInput(from_channel)->AddInputNode(from_node);
  OnInputConnected(from_node);
}


void Node::RemoveInputNode(Channel from_channel, Node * from_node)
{
  GetInput(from_channel)->RemoveInputNode(from_node);
}


void Node::AddOutputNode(Channel to_channel, Node * to_node) // todo: this is useless method
{
  GetOutput(to_channel)->AddOutputNode(to_node);
}


void Node::RemoveOutputNode(Channel to_channel, Node * to_node)
{
  GetOutput(to_channel)->RemoveOutputNode(to_node);
}


void Node::PushInput(Node * pusher, Channel channel, double value)
{
  if(_enabled)
    switch(channel)
      {
      case Channel::Amplitude: GetInput(Channel::Amplitude)->InputMultiply(pusher, value); break;
      case Channel::Form:      GetInput(Channel::Form)->InputAdd(pusher, value);           break;
      case Channel::Aux:       GetInput(Channel::Aux)->InputAdd(pusher, value);            break;
      }
}


void Node::FinishFrame(long time_index)
{
  auto amplitude = GetInput(Channel::Amplitude)->GetValueAndReset();
  //   amplitude = std::clamp(amplitude, 0.0, 1.0);
      
  auto form = GetInput(Channel::Form)->GetValueAndReset();

  assert(_samples_per_second > 0);
  double time = static_cast<double>(time_index) / static_cast<double>(_samples_per_second);

  double result;
  if(_preprocess_amplitude)
    result = ProcessInput(time, amplitude * form);
  else
    result = amplitude * ProcessInput(time, form);

  GetInput(Channel::Aux)->Reset();

  for(auto channel : AllChannels)
    for(auto o : GetOutput(channel)->GetOutputNodes())
      o->PushInput(this, channel, result);

#if LIBFMSYNTH_ENABLE_NODETESTING
  _last_frame = result;
#endif
}


#if LIBFMSYNTH_ENABLE_NODETESTING
double Node::GetLastFrame() const
{
  return _last_frame;
}
#endif


void Node::SetIsFinished()
{
  if(_finished)
    return;
  
  _finished = true;

  std::set<Node *> connected;

  for(const auto & input : _inputs)
    for(auto n : input.GetInputNodes())
      if(n)
        connected.insert(n);

  for(const auto & output : _outputs)
    for(auto n : output.GetOutputNodes())
      if(n)
        connected.insert(n);

  for(auto n : connected)
    n->SetIsFinished();

  OnEOF();
}


bool Node::IsFinished() const
{
  return _finished;
}


std::set<Node *> Node::GetAllOutputNodes() const
{
  std::set<Node *> rv;

  for(const auto & output : _outputs)
    for(auto n : output.GetOutputNodes())
      rv.insert(n);

  return rv;
}

void Node::OnEOF()
{
}


Input * Node::GetInput(Channel channel)
{
  return &_inputs[static_cast<unsigned int>(channel)];
}


const Input * Node::GetInput(Channel channel) const
{
  assert(static_cast<unsigned int>(channel) < _inputs.size());
  return &_inputs[static_cast<unsigned int>(channel)];
}


Output * Node::GetOutput(Channel channel)
{
  return &_outputs[static_cast<unsigned int>(channel)];
}


void Node::ResetTime()
{
  _finished = false;
}


json11::Json Node::to_json() const
{
  return json11::Json::object {
    { "node_id",   _id      },
    { "node_type", _type    },
    { "enabled",   _enabled }
  };
}


void Node::SetFromJson(const json11::Json & json)
{
  // assert(_type == json["node_type"].string_value()); Does not hold because the WidgetNode overwrites node_type.
  _id      = json["node_id"].string_value();
  _enabled = json["enabled"].bool_value();
  UpdateNextId();
}


void Node::UpdateNextId()
{
  auto intid = std::strtoul(_id.c_str(), nullptr, 0);
  if(intid < ULONG_MAX && intid >= _next_id)
    _next_id = intid + 1;
  assert(_next_id > 0);
}


bool Node::IsEnabled() const
{
  return _enabled;
}


void Node::SetEnabled(const Node * root, bool enabled)
{
  assert(root);
  assert(root != this);

  _enabled = enabled;

  if(enabled)
    OnEnabled();
}


void Node::OnEnabled()
{
}


Input::Range Node::GetInputRange(Channel channel) const
{
  return GetInput(channel)->GetInputRange();
}


void Node::SetOutputRange(Input::Range range)
{
  _output_range = range;
}


Input::Range Node::GetFormOutputRange() const
{
  return _output_range;
}


void Node::SetSamplesPerSecond(unsigned int samples_per_second)
{
  _samples_per_second = samples_per_second;
}


unsigned int Node::GetSamplesPerSecond() const
{
  return _samples_per_second;
}
