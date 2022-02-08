#ifndef NODE_ADHSR_HH_
#define NODE_ADHSR_HH_
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
  class NodeADHSR : public Node
  {
  public:
    enum class EndAction {
      STOP,
      RESTART,
      NOP
    };
  
    NodeADHSR();

    void      Set(double attack_time, double decay_time, double hold_time, double sustain_level, double release_time, EndAction end_action);
    double    GetAttackTime()   const;
    double    GetDecayTime()    const;
    double    GetHoldTime()     const;
    double    GetSustainLevel() const;
    double    GetReleaseTime()  const;
    EndAction GetEndAction()    const;

    void         ResetTime()                override;
    Input::Range GetFormOutputRange() const override;
  
    json11::Json to_json() const                        override;
    void         SetFromJson(const json11::Json & json) override;
  
  protected:
    double ProcessInput(double time, double form) override;

  private:
    double    _attack_time;
    double    _decay_time;
    double    _hold_time;
    double    _sustain_level;
    double    _release_time;
    EndAction _end_action;
    double    _timeshift;

    double Attack(double time);
    double Decay(double time);
    double Sustain(double time);
    double Release(double time);
    double End(double time);
  };
}

#endif
