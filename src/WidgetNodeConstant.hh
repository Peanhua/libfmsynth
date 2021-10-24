#ifndef WIDGET_NODE_CONSTANT_HH_
#define WIDGET_NODE_CONSTANT_HH_
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

class NodeConstant;


class WidgetNodeConstant : public WidgetNode
{
public:
  WidgetNodeConstant(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;

private:
  NodeConstant *     _node_constant;
  Ui::NodeConstant * _ui_node_constant;
};


#endif
