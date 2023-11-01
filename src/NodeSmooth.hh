#ifndef NODE_SMOOTH_HH_
#define NODE_SMOOTH_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"


namespace fmsynth
{
  class NodeSmooth : public Node
  {
  public:
    NodeSmooth();

    [[nodiscard]] unsigned int GetWindowSize()            const;
    void                       SetWindowSize(unsigned int size);

    void                       ResetTime()                            override;
    [[nodiscard]] Input::Range GetFormOutputRange() const             override;

    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;

  protected:
    [[nodiscard]] double ProcessInput(double time, double form) override;

  private:
    std::vector<double> _window;
    unsigned int        _position;
    unsigned int        _datasize;
    double              _lastsum;
  };
}

#endif
