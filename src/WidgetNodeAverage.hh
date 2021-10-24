#ifndef WIDGET_NODE_AVERAGE_HH_
#define WIDGET_NODE_AVERAGE_HH_
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

class NodeAverage;


class WidgetNodeAverage : public WidgetNode
{
public:
  WidgetNodeAverage(QWidget * parent);

private:
  NodeAverage *     _node_average;
  Ui::NodeAverage * _ui_node_average;

  void UpdateInfo();
};


#endif
