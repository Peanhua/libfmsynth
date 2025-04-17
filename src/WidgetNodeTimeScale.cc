/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeTimeScale.hh"
#include "NodeTimeScale.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeTimeScale.hh"
#include "QtIncludeEnd.hh"


WidgetNodeTimeScale::WidgetNodeTimeScale(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeTimeScale>(), true, true),
    _node_timescale(dynamic_cast<fmsynth::NodeTimeScale *>(GetNode())),
    _ui_node_timescale(new Ui::NodeTimeScale)
{
  SetNodeType("TimeScale");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_timescale->setupUi(_ui_node->_content);

  _ui_node_timescale->_type->addItem("Linear");
  
  adjustSize();
  ListenWidgetChanges({ _ui_node_timescale->_scale });
}


void WidgetNodeTimeScale::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_timescale->_scale->setValue(_node_timescale->GetScale());
}


void WidgetNodeTimeScale::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_timescale->SetScale(_ui_node_timescale->_scale->value());
}
