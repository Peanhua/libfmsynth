#ifndef WIDGET_NODE_ADHSR_HH_
#define WIDGET_NODE_ADHSR_HH_
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

class NodeADHSR;
class QDoubleSpinBox;
class QSlider;


class WidgetNodeADHSR : public WidgetNode
{
public:
  WidgetNodeADHSR(QWidget * parent);

  void         NodeToWidget() override;
  void         WidgetToNode() override;
  
  json11::Json to_json() const                        override;
  void         SetFromJson(const json11::Json & json) override;

private:
  NodeADHSR *     _node_adhsr;
  Ui::NodeADHSR * _ui_node_adhsr;

  void SetSlidersFromSpinboxes();
  void SetSpinboxesFromSliders();

  void SetSliderFromSpinbox(QDoubleSpinBox * spinbox, QSlider * slider);
  void SetSpinboxFromSlider(QDoubleSpinBox * spinbox, QSlider * slider);

  void UpdateGraph();
};


#endif
