/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"
#include <cassert>

using namespace fmsynth;


unsigned int Node::_next_id = 1;


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
  _inputs[Channel::Amplitude].SetDefaultValue(1);

  _next_id++;
}


Node::~Node()
{
  for(const auto & [channel, input] : _inputs)
    for(auto n : input.GetInputNodes())
      if(n)
        Disconnect(Channel::Form, n, channel, this);

  for(const auto & [channel, output] : _outputs)
    for(auto n : output.GetOutputNodes())
      if(n)
        Disconnect(Channel::Form, this, channel, n);
}


const std::string & Node::GetType() const
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
  _inputs[from_channel].AddInputNode(from_node);
}


void Node::RemoveInputNode(Channel from_channel, Node * from_node)
{
  _inputs[from_channel].RemoveInputNode(from_node);
}


void Node::AddOutputNode(Channel to_channel, Node * to_node)
{
  _outputs[to_channel].AddOutputNode(to_node);
}


void Node::RemoveOutputNode(Channel to_channel, Node * to_node)
{
  _outputs[to_channel].RemoveOutputNode(to_node);
}


void Node::PushInput(Node * pusher, Channel channel, double value)
{
  if(_enabled)
    switch(channel)
      {
      case Channel::Amplitude: _inputs[Channel::Amplitude].InputMultiply(pusher, value); break;
      case Channel::Form:      _inputs[Channel::Form].InputAdd(pusher, value);           break;
      case Channel::Aux:       _inputs[Channel::Aux].InputAdd(pusher, value);            break;
      }
}


void Node::FinishFrame(long time_index)
{
  auto amplitude = _inputs[Channel::Amplitude].GetValueAndReset();
  //   amplitude = std::clamp(amplitude, 0.0, 1.0);
      
  auto form = _inputs[Channel::Form].GetValueAndReset();

  assert(_samples_per_second > 0);
  double time = static_cast<double>(time_index) / static_cast<double>(_samples_per_second);

  double result;
  if(_preprocess_amplitude)
    result = ProcessInput(time, amplitude * form);
  else
    result = amplitude * ProcessInput(time, form);

  _inputs[Channel::Aux].Reset();

  for(const auto & [channel, output] : _outputs)
    for(auto o : output.GetOutputNodes())
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

  for(const auto & [channel, input] : _inputs)
    for(auto n : input.GetInputNodes())
      if(n)
        connected.insert(n);

  for(const auto & [channel, output] : _outputs)
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

  for(const auto & [channel, output] : _outputs)
    for(auto n : output.GetOutputNodes())
      rv.insert(n);

  return rv;
}

void Node::OnEOF()
{
}


Input * Node::GetInput(Channel channel)
{
  return &_inputs[channel];
}


const Input * Node::GetInput(Channel channel) const
{
  assert(_inputs.contains(channel));
  return &_inputs.at(channel);
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
  assert(_inputs.contains(channel));
  return _inputs.at(channel).GetInputRange();
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


void Node::Connect(Channel from_channel, Node * from_node, Channel to_channel, Node * to_node)
{
  assert(from_channel == Channel::Form);
  to_node->AddInputNode(to_channel, from_node);
  if(from_node)
    from_node->AddOutputNode(to_channel, to_node);

  to_node->OnInputConnected(from_node);
}


void Node::Disconnect(Channel from_channel, Node * from_node, Channel to_channel, Node * to_node)
{
  assert(from_channel == Channel::Form);
  to_node->RemoveInputNode(to_channel, from_node);
  if(from_node)
    from_node->RemoveOutputNode(to_channel, to_node);
}
