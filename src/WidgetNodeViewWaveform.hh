#ifndef WIDGET_NODE_VIEW_WAVEFORM_HH_
#define WIDGET_NODE_VIEW_WAVEFORM_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"

namespace fmsynth
{
  class NodeMemoryBuffer;
}


class WidgetNodeViewWaveform : public WidgetNode
{
public:
  WidgetNodeViewWaveform(QWidget * parent);

  void                       NodeToWidget() override;
  void                       WidgetToNode() override;

  [[nodiscard]] json11::Json to_json() const                        override;
  void                       SetFromJson(const json11::Json & json) override;

  void                       timerEvent(QTimerEvent * event)        override;
  
private:
  fmsynth::NodeMemoryBuffer * _node_memorybuffer;
  Ui::NodeViewWaveform *      _ui_node_view_waveform;

  void Redraw();
};


#endif
