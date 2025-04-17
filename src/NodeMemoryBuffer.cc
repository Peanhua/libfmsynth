/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeMemoryBuffer.hh"

using namespace fmsynth;


NodeMemoryBuffer::NodeMemoryBuffer()
  : Node("MemoryBuffer")
{
  SetMaxLength(10);
  SetPreprocessAmplitude();
}


void NodeMemoryBuffer::SetMaxLength(double seconds)
{
  _max_samples = static_cast<unsigned int>(static_cast<double>(GetSamplesPerSecond()) * seconds);
  _buffer.reserve(_max_samples);
}


const std::vector<double> NodeMemoryBuffer::GetData() const
{
  return _buffer;
}


std::mutex & NodeMemoryBuffer::GetLockMutex()
{
  return _mutex;
}


double NodeMemoryBuffer::ProcessInput([[maybe_unused]] double time, double form)
{
  std::lock_guard lock(_mutex);
  if(_buffer.size() < _max_samples)
    _buffer.push_back(form);
  return form;
}


void NodeMemoryBuffer::ResetTime()
{
  _buffer.clear();
}


Input::Range NodeMemoryBuffer::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}
