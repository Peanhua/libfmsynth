#ifndef NODE_OUTPUT_OUTPUT_HH_
#define NODE_OUTPUT_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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
  
  class Output
  {
  public:
    void AddOutputNode(Node * node);
    void RemoveOutputNode(Node * node);
    void RemoveAllOutputNodes();
    
    [[nodiscard]] const std::vector<Node *> & GetOutputNodes() const;

  private:
    std::vector<Node *> _output_nodes;
  };
}

#endif
