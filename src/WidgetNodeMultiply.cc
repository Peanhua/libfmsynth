/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeMultiply.hh"
#include "NodeMultiply.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeMultiply.hh"
#include "QtIncludeEnd.hh"


WidgetNodeMultiply::WidgetNodeMultiply(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeMultiply>(), true, true),
    _node_multiply(dynamic_cast<fmsynth::NodeMultiply *>(GetNode())),
    _ui_node_multiply(new Ui::NodeMultiply)
{
  SetNodeType("Multiply");
  SetIsMultiInput(fmsynth::Node::Channel::Amplitude);
  _ui_node->_input_amplitude->setToolTip("Values in (1 or more)");
  _ui_node->_input_form->setVisible(false);
  _ui_node->_input_form->SetIsOptional();
  SetConnectorsRanges();

  _ui_node_multiply->setupUi(_ui_node->_content);
  adjustSize();
  ListenWidgetChanges({ _ui_node_multiply->_value });
}


void WidgetNodeMultiply::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_multiply->_value->setValue(_node_multiply->GetMultiplier());
}


void WidgetNodeMultiply::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_multiply->SetMultiplier(_ui_node_multiply->_value->value());
}
