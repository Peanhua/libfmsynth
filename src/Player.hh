#ifndef PLAYER_HH_
#define PLAYER_HH_
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
#include <array>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <vector>

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

  bool IsPlaying() const;
  void SetNextProgram(fmsynth::Blueprint * program);

private:
  // Caller thread data:
  std::jthread * _thread;
  bool           _is_playing;

  // Player thread data:
  fmsynth::Blueprint *  _now_playing;
  AudioDevice           _device;

  // Communication between threads:
  std::optional<std::atomic<fmsynth::Blueprint *>> _next_program;
  std::mutex                                       _next_program_mutex;

  void   TickProgramChange();
};

extern Player * ProgramPlayer;

#endif
