#ifndef NODE_RANGE_CONVERT_HH_
#define NODE_RANGE_CONVERT_HH_
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

class Range
{
public:
  Range(double min, double max);
  double ConvertTo(double value, const Range & to);

  double GetMin() const;
  double GetMax() const;

  void Set(double min, double max);
  
private:
  double _min;
  double _max;
};


class NodeRangeConvert : public Node
{
public:
  NodeRangeConvert();

  const Range & GetFrom() const;
  const Range & GetTo()   const;
  void          SetFrom(const Range & range);
  void          SetTo(const Range & range);

  Input::Range GetFormOutputRange() const override;

  json11::Json to_json() const                        override;
  void         SetFromJson(const json11::Json & json) override;
  
protected:
  double ProcessInput(double time, double form);
  
private:
  Range _from;
  Range _to;
};

#endif
