/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeInverse.hh"
#include "NodeInverse.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeInverse.hh"
#include "QtIncludeEnd.hh"


WidgetNodeInverse::WidgetNodeInverse(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeInverse>(), true, true),
    _node_inverse(dynamic_cast<fmsynth::NodeInverse *>(GetNode())),
    _ui_node_inverse(new Ui::NodeInverse)
{
  SetNodeType("Inverse");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_inverse->setupUi(_ui_node->_content);
  adjustSize();
}
