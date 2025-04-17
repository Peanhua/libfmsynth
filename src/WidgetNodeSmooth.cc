/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeSmooth.hh"
#include "NodeSmooth.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeSmooth.hh"
#include "QtIncludeEnd.hh"


WidgetNodeSmooth::WidgetNodeSmooth(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeSmooth>(), true, true),
    _node_smooth(dynamic_cast<fmsynth::NodeSmooth *>(GetNode())),
    _ui_node_smooth(new Ui::NodeSmooth)
{
  SetNodeType("Smooth");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_smooth->setupUi(_ui_node->_content);

  _ui_node_smooth->_type->addItem("Moving average");
  
  adjustSize();
  ListenWidgetChanges({ _ui_node_smooth->_windowsize });
}


void WidgetNodeSmooth::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_smooth->_windowsize->setValue(_node_smooth->GetWindowSize());
}


void WidgetNodeSmooth::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_smooth->SetWindowSize(static_cast<int>(_ui_node_smooth->_windowsize->value()));
}
