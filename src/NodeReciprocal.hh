#ifndef NODE_RECIPROCAL_HH_
#define NODE_RECIPROCAL_HH_
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


namespace fmsynth
{
  class NodeReciprocal : public Node
  {
  public:
    NodeReciprocal();
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form) override;
  };
}

#endif
