/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeAdd.hh"
#include "NodeAdd.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeAdd.hh"
#include "QtIncludeEnd.hh"


WidgetNodeAdd::WidgetNodeAdd(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeAdd>(), true, true),
    _node_add(dynamic_cast<fmsynth::NodeAdd *>(GetNode())),
    _ui_node_add(new Ui::NodeAdd)
{
  SetNodeType("Add");
  SetIsMultiInput(fmsynth::Node::Channel::Form);
  _ui_node->_input_amplitude->setVisible(false);
  SetConnectorsRanges();

  _ui_node_add->setupUi(_ui_node->_content);
  adjustSize();
  ListenWidgetChanges({ _ui_node_add->_value });
}


void WidgetNodeAdd::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_add->SetValue(_ui_node_add->_value->value());
}


void WidgetNodeAdd::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_add->_value->setValue(_node_add->GetValue());
}
