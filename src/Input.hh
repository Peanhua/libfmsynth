#ifndef NODE_INPUT_OUTPUT_HH_
#define NODE_INPUT_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include <vector>

namespace fmsynth
{
  class Node;


  class Input
  {
  public:
    enum class Range
      {
        Inf_Inf,
        MinusOne_One,
        Zero_One
      };
  
    void SetInputRange(Range range);
    void SetDefaultValue(double new_default_value);
  
    void AddInputNode(Node * node);
    void RemoveInputNode(Node * node);
    void RemoveAllInputNodes();

    void   InputAdd(Node * source, double value);
    void   InputMultiply(Node * source, double value);
    double GetValueAndReset();
    double GetValue()       const;
    Range  GetInputRange()  const;
    void   Reset();

    const std::vector<Node *> & GetInputNodes()  const;

  private:
    std::vector<Node *> _input_nodes;
    double       _default_value = 0;
    double       _value         = 0;
    unsigned int _input_count   = 0;
    Range        _input_range   = Range::Inf_Inf;
 
    double NormalizeInputValue(const Node * source, double value) const;
    bool   IsReady()                                              const;
  };
}

#endif
