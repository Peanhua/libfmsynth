#ifndef CONSTANT_VALUE_HH_
#define CONSTANT_VALUE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <json11.hpp>


namespace fmsynth
{
  class ConstantValue
  {
  public:
    enum class Unit
      {
        Absolute,
        Hertz,
        Pi,
        E
      };

    ConstantValue();
    ConstantValue(double value, Unit unit);
  
    double GetValue()     const; // The "raw" value, used internally.
    double GetUnitValue() const; // The value visible in API, UI and .sbp files.
    Unit   GetUnit()      const;

    json11::Json to_json() const;
    void         SetFromJson(const json11::Json & json);

  private:
    double _value = 1;
    Unit   _unit  = Unit::Hertz;
  };
}

#endif
