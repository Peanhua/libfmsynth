/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeRangeConvert.hh"
#include "NodeRangeConvert.hh"
#include <cmath>
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeRangeConvert.hh"
#include "QtIncludeEnd.hh"


WidgetNodeRangeConvert::WidgetNodeRangeConvert(QWidget * parent)
  : WidgetNode(parent, new NodeRangeConvert, true, true),
    _node_rangeconvert(dynamic_cast<NodeRangeConvert *>(GetNode())),
    _ui_node_rangeconvert(new Ui::NodeRangeConvert)
{
  SetNodeType("RangeConvert");
  _ui_node->_title->setText("Range convert");
  _ui_node->_input_amplitude->setVisible(false);
  _ui_node->_input_form->setToolTip("Value in");

  _ui_node_rangeconvert->setupUi(_ui_node->_content);

  _ui_node_rangeconvert->_from->addItem("[0,1]");
  _ui_node_rangeconvert->_from->addItem("[-1,1]");
  _ui_node_rangeconvert->_from->addItem("Custom");
  _ui_node_rangeconvert->_to->addItem("[0,1]");
  _ui_node_rangeconvert->_to->addItem("[-1,1]");
  _ui_node_rangeconvert->_to->addItem("Custom");

  auto SetValidator = [](QLineEdit * widget)
  {
    auto validator = new QDoubleValidator(std::numeric_limits<double>::lowest(),
                               std::numeric_limits<double>::max(),
                               3,
                               widget
                               );
    widget->setValidator(validator);
  };
  SetValidator(_ui_node_rangeconvert->_customfrom_min);
  SetValidator(_ui_node_rangeconvert->_customfrom_max);
  SetValidator(_ui_node_rangeconvert->_customto_min);
  SetValidator(_ui_node_rangeconvert->_customto_max);
  
  adjustSize();


  UpdateRange(_ui_node_rangeconvert->_customfrom, _ui_node_rangeconvert->_from->currentIndex());
  UpdateRange(_ui_node_rangeconvert->_customto,   _ui_node_rangeconvert->_to->currentIndex());
  connect(_ui_node_rangeconvert->_from, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) { UpdateRange(_ui_node_rangeconvert->_customfrom, index); });
  connect(_ui_node_rangeconvert->_to,   QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index) { UpdateRange(_ui_node_rangeconvert->_customto, index);   });

  ListenWidgetChanges({
      _ui_node_rangeconvert->_from,
      _ui_node_rangeconvert->_customfrom_min,
      _ui_node_rangeconvert->_customfrom_max,
      _ui_node_rangeconvert->_to,
      _ui_node_rangeconvert->_customto_min,
      _ui_node_rangeconvert->_customto_max
    });
}


void WidgetNodeRangeConvert::UpdateRange(QWidget * custom, int index)
{
  custom->setVisible(index == 2);
  SetConnectorsRanges();
  adjustSize();
}


void WidgetNodeRangeConvert::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  auto & from = _node_rangeconvert->GetFrom();
  _ui_node_rangeconvert->_customfrom_min->setText(QString::number(from.GetMin()));
  _ui_node_rangeconvert->_customfrom_max->setText(QString::number(from.GetMax()));
  auto & to = _node_rangeconvert->GetTo();
  _ui_node_rangeconvert->_customto_min->setText(QString::number(to.GetMin()));
  _ui_node_rangeconvert->_customto_max->setText(QString::number(to.GetMax()));
}


void WidgetNodeRangeConvert::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  auto GetRange = [this](QWidget * widget, QLineEdit * wmin, QLineEdit * wmax)
  {
    double min = 0, max = 0;
    auto w = dynamic_cast<QComboBox *>(widget);
    assert(w);
    switch(w->currentIndex())
      {
      case 0:
        min = 0;
        max = 1;
        break;
      case 1:
        min = -1;
        max = 1;
        break;
      case 2:
        min = wmin->text().toDouble();
        max = wmax->text().toDouble();
        break;
      }
    if(min >= max)
      max = std::nextafter(min, min + 1);
    
    Range rv(min, max);
    return rv;
  };
  
  auto from = GetRange(_ui_node_rangeconvert->_from, _ui_node_rangeconvert->_customfrom_min, _ui_node_rangeconvert->_customfrom_max);
  auto to   = GetRange(_ui_node_rangeconvert->_to,   _ui_node_rangeconvert->_customto_min,   _ui_node_rangeconvert->_customto_max);
  _node_rangeconvert->SetFrom(from);
  _node_rangeconvert->SetTo(to);
}


json11::Json WidgetNodeRangeConvert::to_json() const
{
  auto rv = WidgetNode::to_json().object_items();
  rv["range_convert_from"] = _ui_node_rangeconvert->_from->currentIndex();
  rv["range_convert_to"]   = _ui_node_rangeconvert->_to->currentIndex();
  auto items = _node_rangeconvert->to_json().object_items();
  for(auto & [k, v] : items)
    rv[k] = v;
  return rv;
}


void WidgetNodeRangeConvert::SetFromJson(const json11::Json & json)
{
  _ui_node_rangeconvert->_from->setCurrentIndex(json["range_convert_from"].int_value());
  _ui_node_rangeconvert->_to->setCurrentIndex(json["range_convert_to"].int_value());
  WidgetNode::SetFromJson(json);
}
