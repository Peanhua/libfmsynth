#ifndef NODE_AUDIO_DEVICE_OUTPUT_HH_
#define NODE_AUDIO_DEVICE_OUTPUT_HH_
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
#include <functional>


namespace fmsynth
{
  class NodeAudioDeviceOutput : public Node
  {
  public:
    typedef std::function<void(NodeAudioDeviceOutput * node, double sample)> play_sample_t;
    typedef std::function<void(NodeAudioDeviceOutput * node)>                continue_playback_t;
  
    NodeAudioDeviceOutput();

    void   SetCallbacks(play_sample_t play_sample, continue_playback_t continue_playback);

    bool   IsMuted() const;
    void   SetMuted(bool muted);

    double GetVolume() const;
    void   SetVolume(double volume);
  
    int16_t SampleToInt(double sample);

    json11::Json to_json() const                        override;
    void         SetFromJson(const json11::Json & json) override;
  
  protected:
    void   OnInputConnected(Node * from)          override;
    void   OnEnabled()                            override;
    double ProcessInput(double time, double form) override;
  
  private:
    double _amplitude;
    bool   _muted;
    play_sample_t       _play_sample;
    continue_playback_t _continue_playback;
  };
}

#endif
