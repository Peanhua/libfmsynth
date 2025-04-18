#ifndef NODE_AUDIO_DEVICE_OUTPUT_HH_
#define NODE_AUDIO_DEVICE_OUTPUT_HH_
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
#include <functional>


namespace fmsynth
{
  class NodeAudioDeviceOutput : public Node
  {
  public:
    typedef std::function<void(double sample)> on_play_sample_t;
  
    NodeAudioDeviceOutput();

    void   SetOnPlaySample(on_play_sample_t callback);

    [[nodiscard]] bool   IsMuted() const;
    void                 SetMuted(bool muted);

    [[nodiscard]] double GetVolume() const;
    void                 SetVolume(double volume);
  
    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form) override;
  
  private:
    double           _amplitude;
    bool             _muted;
    on_play_sample_t _on_play_sample;
  };
}

#endif
