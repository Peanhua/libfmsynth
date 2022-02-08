#ifndef NODE_FILTER_HH_
#define NODE_FILTER_HH_
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


class NodeFilter : public Node
{
public:
  enum class Type {
    LOW_PASS,
    HIGH_PASS
  };

  NodeFilter();

  Type   GetType() const;
  double GetFilterValue() const;
  void   SetType(Type type);
  void   SetFilterValue(double value);

  Input::Range GetAuxInputRange()   const override;
  Input::Range GetFormOutputRange() const override;

  json11::Json to_json() const                        override;
  void         SetFromJson(const json11::Json & json) override;
  
protected:
  double ProcessInput(double time, double form) override;

private:
  Type   _type;
  double _filter;
  
  bool   _first;
  double _lowpass_previous;
  double _highpass_previous_input;
  double _highpass_previous_filtered;

  double LowPass(double filter, double input);
  double HighPass(double filter, double input);
};

#endif
