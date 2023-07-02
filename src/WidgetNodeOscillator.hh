#ifndef WIDGET_NODE_OSCILLATOR_HH_
#define WIDGET_NODE_OSCILLATOR_HH_
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
#include "NodeOscillator.hh"


class WidgetNodeOscillator : public WidgetNode
{
public:
  WidgetNodeOscillator(QWidget * parent, fmsynth::NodeOscillator::Type type);

  void   UpdateConnectorStates() override;
  void   NodeToWidget()          override;
  void   WidgetToNode()          override;

private:
  fmsynth::NodeOscillator::Type _type;
  fmsynth::NodeOscillator *     _node_oscillator;
  Ui::NodeOscillator *          _ui_node_oscillator;

  [[nodiscard]] std::string                   OscillatorTypeString(fmsynth::NodeOscillator::Type type) const;
  [[nodiscard]] fmsynth::NodeOscillator::Type OscillatorTypeFromString(const std::string & string) const;
  void                                        UpdateOscillatorType();
};


#endif
