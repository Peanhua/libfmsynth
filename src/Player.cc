/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Player.hh"
#include "Blueprint.hh"
#include "Node.hh"
#include <cassert>
#include <chrono>
#include <ctime>
#include <iostream>


Player * ProgramPlayer = nullptr;


Player::Player()
  : _thread(nullptr),
    _is_playing(false),
    _now_playing(nullptr),
    _current_buffer(0),
    _current_source(0),
    _resume_playing(false),
    _next_program(std::nullopt)
{
  _sources.fill(AL_NONE);
}


void Player::Start()
{
  _thread = new std::jthread([this](std::stop_token st)
  {
    _now_playing = nullptr;
    
    alGenSources(static_cast<ALsizei>(_sources.size()), _sources.data());
    for(auto s : _sources)
      alSourcei(s, AL_SOURCE_RELATIVE, AL_TRUE);
    assert(alGetError() == AL_NO_ERROR);

    alGenBuffers(static_cast<ALsizei>(_buffers.size()), _buffers.data());
    assert(alGetError() == AL_NO_ERROR);
    
    while(!st.stop_requested())
      {
        TickProgramChange();
        TickSourceQueues();
        TickResumePlaying();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

    for(auto s : _sources)
      {
        alSourceStop(s);
        alSourceUnqueueBuffers(s, 2, _buffers.data());
      }
    alGetError(); // Temporarily ignore: "Unqueueing from a non-streaming source x". Todo: fix it
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    alDeleteBuffers(static_cast<ALsizei>(_buffers.size()), _buffers.data());
    alDeleteSources(static_cast<ALsizei>(_sources.size()), _sources.data());
    assert(alGetError() == AL_NO_ERROR);
    _sources.fill(0);
  });

  assert(!ProgramPlayer);
  ProgramPlayer = this;
}


void Player::Stop()
{
  if(_thread)
    {
      _thread->request_stop();
      _thread->join();
    }
}


void Player::SetNextProgram(fmsynth::Blueprint * program)
{
  _is_playing = program;
  {
    std::lock_guard lock(_next_program_mutex);
    _next_program = std::make_optional(program);
  }
}


void Player::QueueNextBuffer()
{
  if(!_now_playing)
    return;
    
  FillBackBuffer();
  SwapBuffers();
  
  auto buffer = GetCurrentBuffer();
  alSourceQueueBuffers(_sources[_current_source], 1, &buffer);
  assert(alGetError() == AL_NO_ERROR);

  ALint state;
  alGetSourcei(_sources[_current_source], AL_SOURCE_STATE, &state);
}


void Player::TickProgramChange()
{
  fmsynth::Blueprint * newprog = nullptr;
  bool change = false;
  {
    std::lock_guard lock(_next_program_mutex);
    if(_next_program.has_value())
      {
        change = true;
        newprog = _next_program.value();
        _next_program = std::nullopt;
      }
  }
  if(change)
    ChangeProgram(newprog);
}


void Player::ChangeProgram(fmsynth::Blueprint * program)
{
  if(_now_playing && _now_playing != program)
    _now_playing->SetIsFinished();
  
  _current_source ^= 1;
  _now_playing = program;

  auto NewBuffers = [this]()
  {
    for(auto b : _buffers)
      _buffers_to_delete.push_back(b);
    alGenBuffers(static_cast<ALsizei>(_buffers.size()), _buffers.data());
    assert(alGetError() == AL_NO_ERROR);
  };
  
  alSourcef(_sources[_current_source ^ 1], AL_GAIN, 0);

  NewBuffers();
  
  QueueNextBuffer();
  QueueNextBuffer();

  alSourceStop(_sources[_current_source ^ 1]);
  assert(alGetError() == AL_NO_ERROR);
  
  alSourcePlay(_sources[_current_source]);
  assert(alGetError() == AL_NO_ERROR);
  
  alSourcef(_sources[_current_source], AL_GAIN, 1);
}


void Player::TickSourceQueues()
{
  for(auto s : _sources)
    {
      ALint n;
      alGetSourcei(s, AL_BUFFERS_PROCESSED, &n);
      while(n > 0)
        {
          ALuint buffer;
          alSourceUnqueueBuffers(s, 1, &buffer);
          assert(alGetError() == AL_NO_ERROR);
          if(buffer == GetBackBuffer())
            QueueNextBuffer();

          std::vector<ALuint> dbs;
          for(auto b : _buffers_to_delete)
            if(b)
              {
                if(b == buffer)
                  alDeleteBuffers(1, &buffer);
                else
                  dbs.push_back(b);
              }
          _buffers_to_delete = dbs;
          
          n--;
        }
    }
}


void Player::FillBackBuffer()
{
  if(!_now_playing)
    return;

  _data.clear();
  {
    std::lock_guard lock(_now_playing->GetLockMutex());
    _now_playing->Tick(44100 / 9); // = 4900.0
  }
  UpdateBackBuffer(_data);
}

void Player::FillBackbufferValue(int16_t value)
{
  _data.push_back(value);
}


void Player::UpdateBackBuffer(std::vector<int16_t> & data)
{
  auto buffer = _buffers[_current_buffer ^ 1];

  assert(alGetError() == AL_NO_ERROR);
  alBufferData(buffer, AL_FORMAT_MONO16, data.data(), static_cast<ALsizei>(data.size() * sizeof(int16_t)), 44100);
  assert(alGetError() == AL_NO_ERROR);
}


void Player::SwapBuffers()
{
  _current_buffer ^= 1;
}


ALuint Player::GetCurrentBuffer() const
{
  return _buffers[_current_buffer];
}

ALuint Player::GetBackBuffer() const
{
  return _buffers[_current_buffer ^ 1];
}


bool Player::IsPlaying() const
{
  return _is_playing;
}


void Player::Continue()
{
  std::lock_guard lock(_next_program_mutex); // todo: Rename the variable to more generic, or add separate mutex for this.
  _resume_playing = true;
}


void Player::TickResumePlaying()
{
  std::lock_guard lock(_next_program_mutex); // todo: Rename the variable to more generic, or add separate mutex for this.
  if(_resume_playing)
    {
      _resume_playing = false;

      alSourcePlay(_sources[_current_source]);
      assert(alGetError() == AL_NO_ERROR);
    }
}
