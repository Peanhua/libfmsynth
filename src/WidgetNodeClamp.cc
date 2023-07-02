/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeClamp.hh"
#include "NodeClamp.hh"
#include "QtIncludeBegin.hh"
#include <QtGui/QDoubleValidator>
#include "UiNode.hh"
#include "UiNodeClamp.hh"
#include "QtIncludeEnd.hh"


WidgetNodeClamp::WidgetNodeClamp(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeClamp>(), true, true),
    _node_clamp(dynamic_cast<fmsynth::NodeClamp *>(GetNode())),
    _ui_node_clamp(new Ui::NodeClamp)
{
  SetNodeType("Clamp");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");
  SetConnectorsRanges();

  _ui_node_clamp->setupUi(_ui_node->_content);

  auto SetValidator = [](QLineEdit * widget)
  {
    auto validator = new QDoubleValidator(std::numeric_limits<double>::lowest(),
                               std::numeric_limits<double>::max(),
                               3,
                               widget
                               );
    widget->setValidator(validator);
  };
  SetValidator(_ui_node_clamp->_min);
  SetValidator(_ui_node_clamp->_max);

  adjustSize();
}


void WidgetNodeClamp::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_clamp->_min->setText(QString::number(_node_clamp->GetMin()));
  _ui_node_clamp->_max->setText(QString::number(_node_clamp->GetMax()));
}


void WidgetNodeClamp::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_clamp->SetMin(_ui_node_clamp->_min->text().toDouble());
  _node_clamp->SetMax(_ui_node_clamp->_max->text().toDouble());
}
