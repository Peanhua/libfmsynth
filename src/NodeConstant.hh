#ifndef NODE_CONSTANT_HH_
#define NODE_CONSTANT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "ConstantValue.hh"
#include "Node.hh"


namespace fmsynth
{
  class NodeConstant : public Node
  {
  public:
    NodeConstant();

    [[nodiscard]] ConstantValue &       GetValue();
    [[nodiscard]] const ConstantValue & GetValue() const;

    [[nodiscard]] Input::Range GetFormOutputRange() const             override;

    [[nodiscard]] json11::Json to_json() const                        override;
    [[         ]] void         SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form)       override;
  
  private:
    ConstantValue _value;
  };
}

#endif
