/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeAverage.hh"

using namespace fmsynth;


NodeAverage::NodeAverage()
  : Node("Average")
{
  GetInput(Channel::Form)->SetDefaultValue(1);
}


double NodeAverage::ProcessInput([[maybe_unused]] double time, double form)
{
  auto n = static_cast<double>(GetInput(Channel::Form)->GetInputNodes().size());
  if(n > 1)
    return form / n;
  else
    return form;
}


Input::Range NodeAverage::GetFormOutputRange() const
{
  return GetInput(Channel::Form)->GetInputRange();
}
