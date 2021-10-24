/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeAverage.hh"
#include "NodeAverage.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeAverage.hh"
#include "QtIncludeEnd.hh"


WidgetNodeAverage::WidgetNodeAverage(QWidget * parent)
  : WidgetNode(parent, new NodeAverage, true, true),
    _node_average(dynamic_cast<NodeAverage *>(GetNode())),
    _ui_node_average(new Ui::NodeAverage)
{
  SetNodeType("Average");
  SetIsMultiInput("Form");
  _ui_node->_input_amplitude->setVisible(false);
  SetConnectorsRanges();

  _ui_node_average->setupUi(_ui_node->_content);
  adjustSize();
  UpdateInfo();
}

void WidgetNodeAverage::UpdateInfo()
{
  _ui_node_average->_info->setText("");
}
