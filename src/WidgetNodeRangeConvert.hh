#ifndef WIDGET_NODE_RANGECONVERT_HH_
#define WIDGET_NODE_RANGECONVERT_HH_
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

class NodeRangeConvert;


class WidgetNodeRangeConvert : public WidgetNode
{
public:
  WidgetNodeRangeConvert(QWidget * parent);

  void         NodeToWidget() override;
  void         WidgetToNode() override;
  
  json11::Json to_json() const                        override;
  void         SetFromJson(const json11::Json & json) override;

private:
  NodeRangeConvert *     _node_rangeconvert;
  Ui::NodeRangeConvert * _ui_node_rangeconvert;

  void UpdateRange(QWidget * custom, int index);
};


#endif
