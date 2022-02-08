#ifndef WIDGET_NODE_RECIPROCAL_HH_
#define WIDGET_NODE_RECIPROCAL_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"

class NodeReciprocal;


class WidgetNodeReciprocal : public WidgetNode
{
public:
  WidgetNodeReciprocal(QWidget * parent);

private:
  NodeReciprocal *     _node_reciprocal;
  Ui::NodeReciprocal * _ui_node_reciprocal;
};


#endif
