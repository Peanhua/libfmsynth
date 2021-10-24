#ifndef WIDGET_NODE_DELAY_HH_
#define WIDGET_NODE_DELAY_HH_
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

class NodeDelay;


class WidgetNodeDelay : public WidgetNode
{
public:
  WidgetNodeDelay(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;
  
private:
  NodeDelay *     _node_delay;
  Ui::NodeDelay * _ui_node_delay;
};


#endif
