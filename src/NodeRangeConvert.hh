#ifndef NODE_RANGE_CONVERT_HH_
#define NODE_RANGE_CONVERT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"

namespace fmsynth
{
  class Range
  {
  public:
    Range(double min, double max);

    [[nodiscard]] double ConvertTo(double value, const Range & to);

    [[nodiscard]] double GetMin() const;
    [[nodiscard]] double GetMax() const;

    void                 Set(double min, double max);
  
  private:
    double _min;
    double _max;
  };


  class NodeRangeConvert : public Node
  {
  public:
    NodeRangeConvert();

    [[nodiscard]] const Range & GetFrom() const; // todo: Add the "Range" word to these method names: "GetRangeFrom()" etc.
    [[nodiscard]] const Range & GetTo()   const;
    void                        SetFrom(const Range & range);
    void                        SetTo(const Range & range);

    [[nodiscard]] Input::Range GetFormOutputRange() const override;

    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form)       override;
  
  private:
    Range _from;
    Range _to;
  };
}

#endif
