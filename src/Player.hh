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

#include <al.h>
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
  void Continue();
  void SetNextProgram(fmsynth::Blueprint * program);
  void FillBackbufferValue(int16_t value);

private:
  // Caller thread data:
  std::jthread * _thread;
  bool           _is_playing;

  // Player thread data:
  fmsynth::Blueprint *  _now_playing;
  std::array<ALuint, 2> _buffers;
  std::vector<ALuint>   _buffers_to_delete;
  unsigned int          _current_buffer;
  std::vector<int16_t>  _data;

  std::array<ALuint, 2> _sources;
  unsigned int          _current_source;

  // Communication between threads:
  std::atomic<bool>                                _resume_playing;
  std::optional<std::atomic<fmsynth::Blueprint *>> _next_program;
  std::mutex                                       _next_program_mutex;

  void   TickResumePlaying();
  
  void   TickProgramChange();
  void   ChangeProgram(fmsynth::Blueprint * program);

  void   TickSourceQueues();
  void   QueueNextBuffer();

  void   FillBackBuffer();
  void   UpdateBackBuffer(std::vector<int16_t> & data);
  void   SwapBuffers();
  
  ALuint GetCurrentBuffer() const;
  ALuint GetBackBuffer()    const;
};

extern Player * ProgramPlayer;

#endif
