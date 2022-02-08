/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeConstant.hh"
#include "NodeConstant.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeConstant.hh"
#include "QtIncludeEnd.hh"


WidgetNodeConstant::WidgetNodeConstant(QWidget * parent)
  : WidgetNode(parent, new fmsynth::NodeConstant, false, true),
    _node_constant(dynamic_cast<fmsynth::NodeConstant *>(GetNode())),
    _ui_node_constant(new Ui::NodeConstant)
{
  SetNodeType("Constant");
  _ui_node->_input_amplitude->SetIsOptional();
  _ui_node->_input_form->SetIsOptional();
  SetConnectorsRanges();

  _ui_node_constant->setupUi(_ui_node->_content);
  adjustSize();
  ListenWidgetChanges({
      _ui_node_constant->_value,
      _ui_node_constant->_is_frequency
    });
}


void WidgetNodeConstant::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_constant->_value->setValue(_node_constant->GetConstant());
  _ui_node_constant->_is_frequency->setCheckState(_node_constant->IsFrequency() ? Qt::Checked : Qt::Unchecked);
}


void WidgetNodeConstant::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  auto value = _ui_node_constant->_value->value();
  _node_constant->SetConstant(value);
  auto is_freq = _ui_node_constant->_is_frequency->checkState() == Qt::Checked;
  _node_constant->SetIsFrequency(is_freq);
}
