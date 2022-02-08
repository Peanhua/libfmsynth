#ifndef NODE_CONSTANT_HH_
#define NODE_CONSTANT_HH_
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
  class NodeConstant : public Node
  {
  public:
    NodeConstant();

    double GetConstant() const;
    void   SetConstant(double constant);
    bool   IsFrequency() const;
    void   SetIsFrequency(bool is_frequency);

    Input::Range GetFormOutputRange() const override;

    json11::Json to_json() const                        override;
    void         SetFromJson(const json11::Json & json) override;
  
  protected:
    double ProcessInput(double time, double form);
  
  private:
    double _constant;
    bool   _is_frequency;
  };
}

#endif
