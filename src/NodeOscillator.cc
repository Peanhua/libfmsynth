/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeOscillator.hh"
#include <cassert>
#include <numbers>


NodeOscillator::NodeOscillator()
  : Node("Oscillator"),
    _type(Type::SINE),
    _pulse_duty_cycle(0.5),
    _random_generator(0),
    _rdist(0, 1)
{
  GetFormInput()->SetOutputRange(Input::Range::MinusOne_One);
}


NodeOscillator::Type NodeOscillator::GetType() const
{
  return _type;
}


std::string NodeOscillator::TypeToName(Type type) const
{
  switch(type)
    {
    case Type::SINE:     return "Sine";
    case Type::PULSE:    return "Pulse";
    case Type::TRIANGLE: return "Triangle";
    case Type::SAWTOOTH: return "Sawtooth";
    case Type::NOISE:    return "Noise";
    }
  assert(false);
  return "UnknownOscillatorType" + std::to_string(static_cast<int>(type)) + "Error";
}


NodeOscillator::Type NodeOscillator::NameToType(const std::string & type_name) const
{
  if(type_name == "Sine")
    return Type::SINE;
  else if(type_name == "Pulse")
    return Type::PULSE;
  else if(type_name == "Triangle")
    return Type::TRIANGLE;
  else if(type_name == "Sawtooth")
    return Type::SAWTOOTH;
  else //if(type_name == "Noise")
    return Type::NOISE;
}


double NodeOscillator::GetPulseDutyCycle() const
{
  return _pulse_duty_cycle;
}

void NodeOscillator::SetType(Type type)
{
  _type = type;
}

void NodeOscillator::SetPulseDutyCycle(double pulse_duty_cycle)
{
  _pulse_duty_cycle = pulse_duty_cycle;
}



double NodeOscillator::ProcessInput(double time, double form)
{
  switch(_type)
    {
    case Type::SINE:
      return std::sin(form * time);

    case Type::PULSE:
      {
        double duty;
        auto aux = GetAuxInput();
        if(aux->GetInputNodes().size() > 0)
          duty = (aux->GetValue() - 0.5) * 2.0;
        else
          duty = _pulse_duty_cycle * 2.0 - 1.0;
        
        if(duty <= -1)
          return -1;
        return std::sin(form * time) <= duty ? 1 : -1;
      }
    case Type::TRIANGLE:
      return 2.0 / std::numbers::pi * std::asin(std::sin(form * time));

    case Type::SAWTOOTH:
      {
        double rv = 0;
        for(double k = 1; k < 100; k++)
          rv += std::pow(-1.0, k) * std::sin(k * form * time) / k;
        rv = 0.5 - 1.0 / std::numbers::pi * rv;
        // Range for rv at this point is approximately [-0.05, 1.05].
        return rv / 1.05 * 2.0 - 1.0;
      }

    case Type::NOISE:
      return 2.0 * _rdist(_random_generator) - 1.0;
    }
  
  assert(false);
  return form;
}


json11::Json NodeOscillator::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["oscillator_type"] = TypeToName(_type);
  rv["oscillator_pulse_duty_cycle"] = _pulse_duty_cycle;
  return rv;
}


void NodeOscillator::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _type = NameToType(json["oscillator_type"].string_value());
  _pulse_duty_cycle = json["oscillator_pulse_duty_cycle"].number_value();
}
