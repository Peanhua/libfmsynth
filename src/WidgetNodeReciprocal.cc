/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeReciprocal.hh"
#include "NodeReciprocal.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeReciprocal.hh"
#include "QtIncludeEnd.hh"


WidgetNodeReciprocal::WidgetNodeReciprocal(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeReciprocal>(), true, true),
    _node_reciprocal(dynamic_cast<fmsynth::NodeReciprocal *>(GetNode())),
    _ui_node_reciprocal(new Ui::NodeReciprocal)
{
  SetNodeType("Reciprocal");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_reciprocal->setupUi(_ui_node->_content);
  adjustSize();
}
