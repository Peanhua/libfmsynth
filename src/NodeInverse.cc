/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeInverse.hh"
#include <cassert>


NodeInverse::NodeInverse()
  : Node("Inverse")
{
}


double NodeInverse::ProcessInput([[maybe_unused]] double time, double form)
{
  switch(GetFormInput()->GetInputRange())
    {
    case Input::Range::Inf_Inf:
    case Input::Range::MinusOne_One:
      return -form;
    case Input::Range::Zero_One:
      return 1.0 - form;
    }
  assert(false);
  return 0;
}


Input::Range NodeInverse::GetFormOutputRange() const
{
  return GetFormInput()->GetInputRange();
}
