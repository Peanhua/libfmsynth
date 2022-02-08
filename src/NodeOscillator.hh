#ifndef NODE_OSCILLATOR_HH_
#define NODE_OSCILLATOR_HH_
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
#include <random>


namespace fmsynth
{
  class NodeOscillator : public Node
  {
  public:
    enum class Type {
      SINE,
      PULSE,
      TRIANGLE,
      SAWTOOTH,
      NOISE
    };
  
    NodeOscillator();

    Type        GetType()                                 const;
    std::string TypeToName(Type type)                     const;
    Type        NameToType(const std::string & type_name) const;
    double      GetPulseDutyCycle()                       const;
    void        SetType(Type type);
    void        SetPulseDutyCycle(double pulse_duty_cycle);

    json11::Json to_json() const                        override;
    void         SetFromJson(const json11::Json & json) override;
  
  protected:
    double ProcessInput(double time, double form) override;

  private:
    Type   _type;
    double _pulse_duty_cycle; // Percentage signal is high.
    std::mt19937_64                        _random_generator;
    std::uniform_real_distribution<double> _rdist;

  };
}

#endif
