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
  
    Input(double default_value);

    void SetInputRange(Range range);
    void SetOutputRange(Range range);
    void SetDefaultValue(double new_default_value);
  
    void AddInputNode(Node * node);
    void AddOutputNode(Node * node);
    void RemoveInputNode(Node * node);
    void RemoveOutputNode(Node * node);

    void   InputAdd(Node * source, double value);
    void   InputMultiply(Node * source, double value);
    double GetValueAndReset();
    double GetValue()       const;
    Range  GetInputRange()  const;
    Range  GetOutputRange() const;
    void   Reset();

    const std::vector<Node *> & GetInputNodes()  const;
    const std::vector<Node *> & GetOutputNodes() const;

  private:
    std::vector<Node *> _input_nodes;
    std::vector<Node *> _output_nodes;
    double       _default_value;
    double       _value;
    unsigned int _input_count;
    Range        _input_range;
    Range        _output_range;

    double NormalizeInputValue(const Node * source, double value) const;
    bool   IsReady()                                              const;
  };
}

#endif
