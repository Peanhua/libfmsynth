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
    _next_program(std::nullopt)
{
}


void Player::Start()
{
  _thread = new std::jthread([this](std::stop_token st)
  {
    while(!st.stop_requested())
      {
        TickProgramChange();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
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
  _device.Stop();
}


void Player::SetNextProgram(std::shared_ptr<fmsynth::Blueprint> program)
{
  _is_playing = program ? true : false;
  {
    std::lock_guard lock(_next_program_mutex);
    _next_program = std::make_optional(program);
  }
}



void Player::TickProgramChange()
{
  std::shared_ptr<fmsynth::Blueprint> newprog;
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
    {
      _device.Stop();
      _device.Play(newprog);
    }      
}


bool Player::IsPlaying() const
{
  auto bp = _device.GetBlueprint();
  if(!bp)
    return false;
  
  if(bp->IsFinished())
    return false;

  return true;
}


const AudioDevice * Player::GetAudioDevice() const
{
  return &_device;
}

