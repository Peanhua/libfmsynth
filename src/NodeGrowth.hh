#ifndef NODE_GROWTH_HH_
#define NODE_GROWTH_HH_
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
  class NodeGrowth : public Node
  {
  public:
    enum class Formula { Linear, Logistic, Exponential };
    enum class EndAction { NoEnd, RepeatLast, RestartFromStart, Stop };
    
    NodeGrowth();

    [[nodiscard]] ConstantValue & ParamStartValue()    { return _start_value;    }
    [[nodiscard]] Formula &       ParamGrowthFormula() { return _growth_formula; }
    [[nodiscard]] ConstantValue & ParamGrowthAmount()  { return _growth_amount;  }
    [[nodiscard]] EndAction &     ParamEndAction()     { return _end_action;     }
    [[nodiscard]] ConstantValue & ParamEndValue()      { return _end_value;      }

    void                       ResetTime()                            override;
    [[nodiscard]] Input::Range GetFormOutputRange() const             override;

    [[nodiscard]] json11::Json to_json() const                        override;
    void                       SetFromJson(const json11::Json & json) override;
  
  protected:
    [[nodiscard]] double ProcessInput(double time, double form)       override;
  
  private:
    ConstantValue _start_value;
    Formula       _growth_formula;
    ConstantValue _growth_amount;
    EndAction     _end_action;
    ConstantValue _end_value;
    double        _start_time;
    double        _current_value;

    void UpdateUiVisibility();
  };
}

#endif
