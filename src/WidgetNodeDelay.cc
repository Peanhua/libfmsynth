/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeDelay.hh"
#include "NodeDelay.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeDelay.hh"
#include "QtIncludeEnd.hh"


WidgetNodeDelay::WidgetNodeDelay(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeDelay>(), true, true),
    _node_delay(dynamic_cast<fmsynth::NodeDelay *>(GetNode())),
    _ui_node_delay(new Ui::NodeDelay)
{
  SetNodeType("Delay");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_delay->setupUi(_ui_node->_content);
  adjustSize();
  ListenWidgetChanges({ _ui_node_delay->_delay });
}


void WidgetNodeDelay::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_delay->_delay->setValue(_node_delay->GetDelayTime());
}


void WidgetNodeDelay::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_delay->SetDelayTime(_ui_node_delay->_delay->value());
}
