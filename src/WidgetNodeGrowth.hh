#ifndef WIDGET_NODE_GROWTH_HH_
#define WIDGET_NODE_GROWTH_HH_
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

namespace fmsynth
{
  class NodeGrowth;
}


class WidgetNodeGrowth : public WidgetNode
{
public:
  WidgetNodeGrowth(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;

private:
  fmsynth::NodeGrowth * _node_growth;
  Ui::NodeGrowth *      _ui_node_growth;

  void UpdateUiVisibility();
};


#endif
