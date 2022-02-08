#ifndef NODE_DELAY_HH_
#define NODE_DELAY_HH_
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
#include <deque>

class NodeDelay : public Node
{
public:
  NodeDelay();

  double GetDelayTime()            const;
  void   SetDelayTime(double time);

  void         ResetTime()                override;
  Input::Range GetFormOutputRange() const override;
  
  json11::Json to_json() const                        override;
  void         SetFromJson(const json11::Json & json) override;
  
protected:
  double ProcessInput(double time, double form);
  
private:
  double             _delay_time;
  std::deque<double> _buffer;

  void PrefillBuffer();
};

#endif
