#ifndef PLAYER_HH_
#define PLAYER_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "AudioDevice.hh"
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

class AudioDevice;
namespace fmsynth
{
  class Blueprint;
}


class Player
{
public:
  Player();

  void Start();
  void Stop();

  void                              SetAudioDevice(int device_id);
  [[nodiscard]] const AudioDevice * GetAudioDevice() const;
  [[nodiscard]] bool                IsPlaying() const;
  void                              SetNextProgram(std::shared_ptr<fmsynth::Blueprint> program);

private:
  // Caller thread data:
  std::jthread * _thread;
  bool           _is_playing;

  // Player thread data:
  std::shared_ptr<fmsynth::Blueprint> _now_playing;
  AudioDevice                         _device;

  // Communication between threads:
  std::optional<std::shared_ptr<fmsynth::Blueprint>> _next_program;
  std::mutex                                         _next_program_mutex;

  void   TickProgramChange();
};

extern Player * ProgramPlayer;

#endif
