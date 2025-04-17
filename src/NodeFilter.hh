#ifndef NODE_FILTER_HH_
#define NODE_FILTER_HH_
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
  class NodeFilter : public Node
  {
  public:
    enum class Type {
      LOW_PASS,
      HIGH_PASS
    };

    NodeFilter();

    [[nodiscard]] Type   GetFilterType()  const;
    [[nodiscard]] double GetFilterValue() const;
    void                 SetFilterType(Type type);
    void                 SetFilterValue(double value);

    [[nodiscard]] Input::Range GetInputRange(Channel channel) const override;
    [[nodiscard]] Input::Range GetFormOutputRange() const override;

    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form) override;

  private:
    Type   _type;
    double _filter;
  
    bool   _first;
    double _lowpass_previous;
    double _highpass_previous_input;
    double _highpass_previous_filtered;

    [[nodiscard]] double LowPass(double filter, double input);
    [[nodiscard]] double HighPass(double filter, double input);
  };
}

#endif
