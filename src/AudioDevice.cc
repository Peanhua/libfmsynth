/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "AudioDevice.hh"
#include "Blueprint.hh"
#include "NodeAudioDeviceOutput.hh"
#include <cassert>
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wdeprecated-copy"
# pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <RtAudio.h>
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif


static RtAudio * GetDAC()
{
  auto getit = [](RtAudio::Api api) -> RtAudio *
  {
    try
      {
        RtAudio * rv;
        if(api == RtAudio::Api::UNSPECIFIED)
          rv = new RtAudio();
        else
          rv = new RtAudio(api);
        if(rv)
          {
            if((api == RtAudio::Api::UNSPECIFIED || api == rv->getCurrentApi()) && rv->getDeviceCount() > 0)
              return rv;
            else
              delete rv;
          }
      }
    catch(RtAudioError & e)
      {
      }

    return nullptr;
  };

  
  std::vector<RtAudio::Api> apis
    {
#ifdef __linux__
      RtAudio::Api::LINUX_PULSE,
#endif
      RtAudio::Api::UNSPECIFIED
    };
  for(auto api : apis)
    {
      auto rv = getit(api);
      if(rv)
        return rv;
    }

  assert(false);
  return nullptr;
}





AudioDevice::AudioDevice()
  : _on_post_tick(nullptr),
    _blueprint(nullptr)    
{
  _dac = GetDAC();

  if(_dac)
    {
      _device_id = _dac->getDefaultOutputDevice();
      // Try to fix case when _dac->getDefaultOutputDevice() sometimes incorrectly returns 0:
      for(unsigned int i = 0; _device_id == 0 && i < _dac->getDeviceCount(); i++)
        try
          {
            auto info = _dac->getDeviceInfo(i);
            if(info.outputChannels > 0)
              _device_id = i;
          }
        catch(const std::exception & e)
          {
          }

      auto deviceinfo = _dac->getDeviceInfo(_device_id);
      _sample_rates = deviceinfo.sampleRates;
    }
}


AudioDevice::~AudioDevice()
{
  delete _dac;
}


const std::vector<unsigned int> & AudioDevice::GetSampleRates() const
{
  return _sample_rates;
}


void AudioDevice::Playback(double * output_buffer, unsigned int frame_count)
{
  if(!_blueprint || _nodes.size() == 0)
    return;
  
  std::lock_guard lock(_blueprint->GetLockMutex());
  
  for(unsigned i = 0; !_blueprint->IsFinished() && i < frame_count; i++)
    {
      _current_sample = 0;
      _blueprint->Tick(1);
      *output_buffer++ = _current_sample;

      if(_on_post_tick)
        _on_post_tick(_current_sample);
    }
}


std::shared_ptr<fmsynth::Blueprint> AudioDevice::GetBlueprint()
{
  return _blueprint;
}


const std::shared_ptr<fmsynth::Blueprint> AudioDevice::GetBlueprint() const
{
  return _blueprint;
}


void AudioDevice::SetOnPostTick(on_post_tick_t callback)
{
  _on_post_tick = callback;
}


void AudioDevice::Play(std::shared_ptr<fmsynth::Blueprint> blueprint)
{
  _blueprint = blueprint;
  UpdateInputNodes();

  if(!_blueprint)
    return;
  
  _blueprint->ResetTime();
  
  RtAudio::StreamParameters parameters;
  parameters.nChannels    = 1;
  parameters.firstChannel = 0;
  parameters.deviceId     = _device_id;

  try
    {
      static std::function<int(double *, unsigned int)> callback = [this](double * output_buffer, unsigned int frame_count) -> int
      { // The purpose of this lambda (extra redirection) is to be able to use the private method (Playback) on this object.
        Playback(output_buffer, frame_count);
        if(_blueprint->IsFinished())
          return 1;
        return 0;
      };
      
      unsigned int bframes = 1024;
      _dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT64, _blueprint->GetSamplesPerSecond(), &bframes,
                       [](void *                  outputBuffer,
                          [[maybe_unused]] void * inputBuffer,
                          unsigned int            nBufferFrames,
                          [[maybe_unused]] double streamTime,
                          RtAudioStreamStatus     status,
                          void *                  userData) -> int
                       {
                         if(status)
                           return 1;
                         auto c = static_cast<std::function<int(double *, unsigned int)> *>(userData);
                         return (*c)(static_cast<double *>(outputBuffer), nBufferFrames);
                       },
                       &callback);
      _dac->startStream();
    }
  catch(RtAudioError & e)
    {
      e.printMessage();
    }
}      




void AudioDevice::Stop()
{
  try
    {
      _dac->closeStream();
    }
  catch(RtAudioError & e)
    {
    }
}


void AudioDevice::UpdateInputNodes()
{
  _nodes.clear();

  if(!_blueprint)
    return;
  
  auto ados = _blueprint->GetNodesByType("AudioDeviceOutput");
  for(auto n : ados)
    _nodes.push_back(dynamic_cast<fmsynth::NodeAudioDeviceOutput *>(n));

  for(auto n : _nodes)
    n->SetOnPlaySample(
                       [this](double sample)
                       {
                         _current_sample += sample;
                       }
                       );
}


const std::string AudioDevice::GetDeviceName() const
{
  auto info = _dac->getDeviceInfo(_device_id);
  return info.name;
}


const std::vector<fmsynth::NodeAudioDeviceOutput *> AudioDevice::GetInputNodes() const
{
  return _nodes;
}
