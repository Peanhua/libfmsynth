/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeAudioDeviceOutput.hh"


NodeAudioDeviceOutput::NodeAudioDeviceOutput()
  : Node("AudioDeviceOutput"),
    _amplitude(1),
    _muted(false),
    _play_sample(nullptr),
    _continue_playback(nullptr)
{
  SetPreprocessAmplitude();
}


void NodeAudioDeviceOutput::SetCallbacks(play_sample_t play_sample, continue_playback_t continue_playback)
{
  _play_sample       = play_sample;
  _continue_playback = continue_playback;
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

  if(_play_sample)
    _play_sample(this, _amplitude * form);
  
  return form;
}


int16_t NodeAudioDeviceOutput::SampleToInt(double sample)
{
  sample = std::clamp(sample, -1.0, 1.0);
  if(sample < 0)
    sample *= static_cast<double>(std::abs(INT16_MIN));
  else
    sample *= static_cast<double>(INT16_MAX);
  
  return static_cast<int16_t>(sample);
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


void NodeAudioDeviceOutput::OnInputConnected(Node * from)
{
  Node::OnInputConnected(from);
  if(_continue_playback)
    _continue_playback(this);
}


void NodeAudioDeviceOutput::OnEnabled()
{
  Node::OnEnabled();
  if(_continue_playback)
    _continue_playback(this);
}
