#ifndef WIDGET_NODE_INVERSE_HH_
#define WIDGET_NODE_INVERSE_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"

class NodeInverse;


class WidgetNodeInverse : public WidgetNode
{
public:
  WidgetNodeInverse(QWidget * parent);

private:
  NodeInverse *     _node_inverse;
  Ui::NodeInverse * _ui_node_inverse;
};


#endif
