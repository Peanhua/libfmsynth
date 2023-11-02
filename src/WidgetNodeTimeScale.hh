#ifndef WIDGET_NODE_TIME_SCALE_HH_
#define WIDGET_NODE_TIME_SCALE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"


namespace fmsynth
{
  class NodeTimeScale;
}


class WidgetNodeTimeScale : public WidgetNode
{
public:
  WidgetNodeTimeScale(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;
  
private:
  fmsynth::NodeTimeScale * _node_timescale;
  Ui::NodeTimeScale *      _ui_node_timescale;
};


#endif
