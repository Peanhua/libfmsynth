#ifndef NODE_MEMORY_BUFFER_HH_
#define NODE_MEMORY_BUFFER_HH_
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
#include <mutex>
#include <vector>


namespace fmsynth
{
  class NodeMemoryBuffer : public Node
  {
  public:
    NodeMemoryBuffer();

    void                                    SetMaxLength(double seconds);
    void                                    Clear();
    [[nodiscard]] const std::vector<double> GetData() const;
    [[nodiscard]] std::mutex &              GetLockMutex();

    void                                    ResetTime() override;
    [[nodiscard]] Input::Range              GetFormOutputRange() const override;

  protected:
    unsigned int        _max_samples;
    std::vector<double> _buffer;
    std::mutex          _mutex;
  
    [[nodiscard]] double ProcessInput(double time, double form) override;
  
  private:
  };
}

#endif
