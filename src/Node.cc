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
    _amplitude(1),
    _form(0),
    _aux(0)
#if LIBFMSYNTH_ENABLE_NODETESTING
  , _last_frame(0)
#endif
{
  _next_id++;
}


Node::~Node()
{
  for(auto n : _amplitude.GetInputNodes())
    if(n)
      RemoveAmplitudeInputNode(n);
  for(auto n : _form.GetInputNodes())
    if(n)
      RemoveFormInputNode(n);
  for(auto n : _aux.GetInputNodes())
    if(n)
      RemoveAuxInputNode(n);
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


void Node::AddInputNode(Channel channel, Node * node)
{
  switch(channel)
    {
    case Channel::Amplitude: AddAmplitudeInputNode(node); break;
    case Channel::Form:      AddFormInputNode(node);      break;
    case Channel::Aux:       AddAuxInputNode(node);       break;
    }
}


void Node::RemoveInputNode(Channel channel, Node * node)
{
  switch(channel)
    {
    case Channel::Amplitude: RemoveAmplitudeInputNode(node); break;
    case Channel::Form:      RemoveFormInputNode(node);      break;
    case Channel::Aux:       RemoveAuxInputNode(node);       break;
    }
}


void Node::AddAmplitudeInputNode(Node * node)
{
  _amplitude.AddInputNode(node);
  if(node)
    node->_amplitude.AddOutputNode(this);
  OnInputConnected(node);
}

void Node::AddFormInputNode(Node * node)
{
  _form.AddInputNode(node);
  if(node)
    node->_form.AddOutputNode(this);
  OnInputConnected(node);
}

void Node::AddAuxInputNode(Node * node)
{
  _aux.AddInputNode(node);
  if(node)
    node->_aux.AddOutputNode(this);
  OnInputConnected(node);
}

void Node::RemoveAmplitudeInputNode(Node * node)
{
  _amplitude.RemoveInputNode(node);
  if(node)
    node->_amplitude.RemoveOutputNode(this);
}

void Node::RemoveFormInputNode(Node * node)
{
  _form.RemoveInputNode(node);
  if(node)
    node->_form.RemoveOutputNode(this);
}
  
void Node::RemoveAuxInputNode(Node * node)
{
  _aux.RemoveInputNode(node);
  if(node)
    node->_aux.RemoveOutputNode(this);
}


void Node::PushAmplitudeInput(long time_index, Node * pusher, double amplitude)
{
  if(_enabled)
    {
      _amplitude.InputMultiply(pusher, amplitude);
      FinishFrame(time_index);
    }
}

void Node::PushInput(long time_index, Node * pusher, Channel channel, double value)
{
  switch(channel)
    {
    case Channel::Amplitude: PushAmplitudeInput(time_index, pusher, value); break;
    case Channel::Form:      PushFormInput(time_index, pusher, value);      break;
    case Channel::Aux:       PushAuxInput(time_index, pusher, value);       break;
    }
}

  
void Node::PushFormInput(long time_index, Node * pusher, double form)
{
  if(_enabled)
    {
      _form.InputAdd(pusher, form);
      FinishFrame(time_index);
    }
}

void Node::PushAuxInput(long time_index, Node * pusher, double value)
{
  if(_enabled)
    {
      _aux.InputAdd(pusher, value); // todo: Allow toggling between add/multiply (if needed, this is only used by the filter node as of writing).
      FinishFrame(time_index);
    }
}


void Node::FinishFrame(long time_index)
{
  if(_amplitude.IsReady() && _form.IsReady() && _aux.IsReady())
    {
      auto amplitude = _amplitude.GetValueAndReset();
      //   amplitude = std::clamp(amplitude, 0.0, 1.0);
      
      auto form = _form.GetValueAndReset();

      assert(_samples_per_second > 0);
      double time = static_cast<double>(time_index) / static_cast<double>(_samples_per_second);

      double result;
      if(_preprocess_amplitude)
        result = ProcessInput(time, amplitude * form);
      else
        result = amplitude * ProcessInput(time, form);

      _aux.Reset();
      
      for(auto o : _amplitude.GetOutputNodes())
        o->PushAmplitudeInput(time_index, this, result);
      
      for(auto o : _form.GetOutputNodes())
        o->PushFormInput(time_index, this, result);

      for(auto o : _aux.GetOutputNodes())
        o->PushAuxInput(time_index, this, result);

#if LIBFMSYNTH_ENABLE_NODETESTING
      _last_frame = result;
#endif
    }
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
  std::array<Input *, 3> inputs {
    &_amplitude,
    &_form,
    &_aux
  };
  for(auto i : inputs)
    {
      for(auto n : i->GetInputNodes())
        if(n)
          connected.insert(n);
      for(auto n : i->GetOutputNodes())
        if(n)
          connected.insert(n);
    }

  for(auto n : connected)
    n->SetIsFinished();

  OnEOF();
}


bool Node::IsFinished() const
{
  return _finished;
}


std::set<Node *> Node::GetAllConnectedNodes() const
{
  std::set<Node *> rv;

  for(auto n : _amplitude.GetOutputNodes())
    rv.insert(n);
  for(auto n : _form.GetOutputNodes())
    rv.insert(n);
  for(auto n : _aux.GetOutputNodes())
    rv.insert(n);

  return rv;
}

void Node::OnEOF()
{
}


Input * Node::GetInput(Channel channel)
{
  switch(channel)
    {
    case Channel::Amplitude: return &_amplitude;
    case Channel::Form:      return &_form;
    case Channel::Aux:       return &_aux;
    }
  assert(false);
  return nullptr;
}


const Input * Node::GetInput(Channel channel) const
{
  switch(channel)
    {
    case Channel::Amplitude: return &_amplitude;
    case Channel::Form:      return &_form;
    case Channel::Aux:       return &_aux;
    }
  assert(false);
  return nullptr;
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
  switch(channel)
    {
    case Channel::Amplitude: return _amplitude.GetInputRange();
    case Channel::Form:      return _form.GetInputRange();
    case Channel::Aux:       return _aux.GetInputRange();
    }
  assert(false);
  return Input::Range::Zero_One;
}


Input::Range Node::GetFormOutputRange() const
{
  return _form.GetOutputRange();
}


void Node::SetSamplesPerSecond(unsigned int samples_per_second)
{
  _samples_per_second = samples_per_second;
}


unsigned int Node::GetSamplesPerSecond() const
{
  return _samples_per_second;
}

