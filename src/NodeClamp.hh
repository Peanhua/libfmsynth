#ifndef NODE_CLAMP_HH_
#define NODE_CLAMP_HH_
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
  class NodeClamp : public Node
  {
  public:
    NodeClamp();

    [[nodiscard]] double GetMin() const;
    [[nodiscard]] double GetMax() const;
    void                 SetMin(double min);
    void                 SetMax(double max);

    [[nodiscard]] Input::Range GetFormOutputRange() const             override;

    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form)       override;
  
  private:
    double _min;
    double _max;
  };
}

#endif
