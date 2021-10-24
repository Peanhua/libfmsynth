/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeADHSR.hh"
#include <cassert>
#include <cmath>


NodeADHSR::NodeADHSR()
  : Node("ADHSR"),
    _attack_time(0),
    _decay_time(0),
    _hold_time(0),
    _sustain_level(1),
    _release_time(0),
    _end_action(EndAction::STOP),
    _timeshift(0)
{
}



void NodeADHSR::Set(double attack_time, double decay_time, double hold_time, double sustain_level, double release_time, EndAction end_action)
{
  _attack_time   = attack_time;
  _decay_time    = decay_time;
  _hold_time     = hold_time;
  _sustain_level = sustain_level;
  _release_time  = release_time;
  _end_action    = end_action;
}


double NodeADHSR::GetAttackTime() const
{
  return _attack_time;
}


double NodeADHSR::GetDecayTime() const
{
  return _decay_time;
}


double NodeADHSR::GetHoldTime() const
{
  return _hold_time;
}


double NodeADHSR::GetSustainLevel() const
{
  return _sustain_level;
}


double NodeADHSR::GetReleaseTime() const
{
  return _release_time;
}


NodeADHSR::EndAction NodeADHSR::GetEndAction() const
{
  return _end_action;
}


void NodeADHSR::ResetTime()
{
  Node::ResetTime();
  _timeshift = 0;
}



double NodeADHSR::ProcessInput(double time, double form)
{
  if(_attack_time + _decay_time + _hold_time + _release_time <= 0)
    return 0;
  
  assert(time >= _timeshift);
  time -= _timeshift;

  double m = 0;
  if(time < _attack_time)
    m = Attack(time);
  else if(time < _attack_time + _decay_time)
    m = Decay(time);
  else if(time < _attack_time + _decay_time + _hold_time)
    m = Sustain(time);
  else if(time < _attack_time + _decay_time + _hold_time + _release_time)
    m = Release(time);
  else
    m = End(time);

  return form * m;
}


double NodeADHSR::Attack(double time)
{
  return time / _attack_time;
}


double NodeADHSR::Decay(double time)
{
  auto pos = (time - _attack_time) / _decay_time;
  return 1.0 - pos * (1.0 - _sustain_level);
}


double NodeADHSR::Sustain([[maybe_unused]] double time)
{
  return _sustain_level;
}


double NodeADHSR::Release(double time)
{
  double pos = (time - _attack_time - _decay_time - _hold_time) / _release_time;
  return std::lerp(_sustain_level, 0.0, pos);
}


double NodeADHSR::End(double time)
{
  double rv = 0;
  switch(_end_action)
    {
    case EndAction::STOP:
      SetIsFinished();
      break;
    case EndAction::RESTART:
      _timeshift += time;
      rv = ProcessInput(_timeshift, 1);
      break;
    case EndAction::NOP:
      break;
    }
  return rv;
}


json11::Json NodeADHSR::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["adhsr_attack"]     = _attack_time;
  rv["adhsr_decay"]      = _decay_time;
  rv["adhsr_hold"]       = _hold_time;
  rv["adhsr_sustain"]    = _sustain_level;
  rv["adhsr_release"]    = _release_time;
  rv["adhsr_end_action"] = static_cast<int>(_end_action);
  return rv;
}


void NodeADHSR::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _attack_time   = json["adhsr_attack"].number_value();
  _decay_time    = json["adhsr_decay"].number_value();
  _hold_time     = json["adhsr_hold"].number_value();
  _sustain_level = json["adhsr_sustain"].number_value();
  _release_time  = json["adhsr_release"].number_value();
  _end_action    = static_cast<EndAction>(json["adhsr_end_action"].int_value());
}


Input::Range NodeADHSR::GetFormOutputRange() const
{
  return GetFormInput()->GetInputRange();
}
