#ifndef AUDIODEVICE_HH_
#define AUDIODEVICE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <functional>
#include <string>
#include <vector>


namespace fmsynth
{
  class Blueprint;
  class NodeAudioDeviceOutput;
}
class RtAudio;


class AudioDevice
{
public:
  typedef std::function<void(double sample)> on_post_tick_t;

  AudioDevice();
  ~AudioDevice();

  void SetOnPostTick(on_post_tick_t callback);
  void Play(fmsynth::Blueprint * blueprint);
  void Stop();

  const std::string                                   GetDeviceName()  const;
  const std::vector<unsigned int> &                   GetSampleRates() const;
  fmsynth::Blueprint *                                GetBlueprint();
  const fmsynth::Blueprint *                          GetBlueprint()   const;
  const std::vector<fmsynth::NodeAudioDeviceOutput *> GetInputNodes()  const;
  
private:
  RtAudio *                 _dac;
  unsigned int              _device_id;
  std::vector<unsigned int> _sample_rates;
  on_post_tick_t            _on_post_tick;
  double                    _current_sample;
  fmsynth::Blueprint *                          _blueprint;
  std::vector<fmsynth::NodeAudioDeviceOutput *> _nodes;

  void UpdateInputNodes();
  void Playback(double * output_buffer, unsigned int frame_count);
};

#endif
