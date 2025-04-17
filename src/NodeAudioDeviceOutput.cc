/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeAudioDeviceOutput.hh"

using namespace fmsynth;


NodeAudioDeviceOutput::NodeAudioDeviceOutput()
  : Node("AudioDeviceOutput"),
    _amplitude(1),
    _muted(false),
    _on_play_sample(nullptr)
{
  GetInput(Channel::Form)->SetInputRange(Input::Range::MinusOne_One);
  SetPreprocessAmplitude();
}


void NodeAudioDeviceOutput::SetOnPlaySample(on_play_sample_t callback)
{
  _on_play_sample = callback;
}


bool NodeAudioDeviceOutput::IsMuted() const
{
  return _muted;
}


void NodeAudioDeviceOutput::SetMuted(bool muted)
{
  _muted = muted;
}


double NodeAudioDeviceOutput::GetVolume() const
{
  return _amplitude;
}

void NodeAudioDeviceOutput::SetVolume(double volume)
{
  _amplitude = volume;
}


double NodeAudioDeviceOutput::ProcessInput([[maybe_unused]] double time, double form)
{
  if(_muted)
    return 0;

  if(_on_play_sample)
    _on_play_sample(_amplitude * form);
  
  return form;
}


json11::Json NodeAudioDeviceOutput::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["audiodeviceoutput_muted"]  = _muted;
  rv["audiodeviceoutput_volume"] = _amplitude;
  return rv;
}

void NodeAudioDeviceOutput::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _muted     = json["audiodeviceoutput_muted"].bool_value();
  _amplitude = json["audiodeviceoutput_volume"].number_value();
}
