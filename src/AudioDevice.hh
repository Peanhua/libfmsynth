#ifndef AUDIODEVICE_HH_
#define AUDIODEVICE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <functional>
#include <memory>
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

  AudioDevice(int device_id);
  ~AudioDevice();

  void SetDeviceId(int device_id); // -1 for the default device
  void SetOnPostTick(on_post_tick_t callback);
  void Play(std::shared_ptr<fmsynth::Blueprint> blueprint);
  void Stop();

  [[nodiscard]] std::string                                         GetDeviceName()      const;
  [[nodiscard]] std::string                                         GetDeviceName(unsigned int device_id) const;
  [[nodiscard]] const std::vector<unsigned int> &                   GetDeviceIds()       const;
  [[nodiscard]] unsigned int                                        GetDefaultDeviceId() const;
  [[nodiscard]] const std::vector<unsigned int> &                   GetSampleRates()     const;
  [[nodiscard]] std::shared_ptr<fmsynth::Blueprint>                 GetBlueprint();
  [[nodiscard]] const std::shared_ptr<fmsynth::Blueprint>           GetBlueprint()       const;
  [[nodiscard]] const std::vector<fmsynth::NodeAudioDeviceOutput *> GetInputNodes()      const;
 
  void Playback(double * output_buffer, unsigned int frame_count);
 
private:
  RtAudio *                 _dac;
  unsigned int              _device_id;
  std::vector<unsigned int> _device_ids;
  std::vector<unsigned int> _sample_rates;
  on_post_tick_t            _on_post_tick;
  double                    _current_sample;
  std::shared_ptr<fmsynth::Blueprint>           _blueprint;
  std::vector<fmsynth::NodeAudioDeviceOutput *> _nodes;

  void UpdateInputNodes();
  void UpdateDeviceNames();
};

#endif
