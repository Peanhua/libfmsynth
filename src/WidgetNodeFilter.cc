/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeFilter.hh"
#include "NodeFilter.hh"
#include <cassert>
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeFilter.hh"
#include "QtIncludeEnd.hh"


WidgetNodeFilter::WidgetNodeFilter(QWidget * parent)
  : WidgetNodeFilter(parent, fmsynth::NodeFilter::Type::LOW_PASS)
{
}


WidgetNodeFilter::WidgetNodeFilter(QWidget * parent, fmsynth::NodeFilter::Type type)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeFilter>(), true, true),
    _type(type),
    _node_filter(dynamic_cast<fmsynth::NodeFilter *>(GetNode())),
    _ui_node_filter(new Ui::NodeFilter)
{
  AddAuxInput();
  _ui_node->_input_aux->setToolTip("Filter value in");
  SetConnectorsRanges();

  _ui_node_filter->setupUi(_ui_node->_content);

  _ui_node_filter->_type->addItem("Low pass");
  _ui_node_filter->_type->addItem("High pass");
  UpdateFilterType();

  connect(_ui_node_filter->_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index)
          {
            switch(index)
              {
              case 0: _type = fmsynth::NodeFilter::Type::LOW_PASS;  break;
              case 1: _type = fmsynth::NodeFilter::Type::HIGH_PASS; break;
              }
            UpdateFilterType();
          });
  
  adjustSize();
  ListenWidgetChanges({
      _ui_node_filter->_type,
      _ui_node_filter->_value
    });
}


void WidgetNodeFilter::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _type = _node_filter->GetType();
  _ui_node_filter->_value->setValue(_node_filter->GetFilterValue());
  UpdateFilterType();
}


void WidgetNodeFilter::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_filter->SetType(_type);
  _node_filter->SetFilterValue(_ui_node_filter->_value->value());
}


void WidgetNodeFilter::UpdateConnectorStates()
{
  WidgetNode::UpdateConnectorStates();
  bool isconnected = _ui_node->_input_aux->IsConnected();
  _ui_node_filter->_value->setEnabled(!isconnected);
}


std::string WidgetNodeFilter::FilterTypeString(fmsynth::NodeFilter::Type type) const
{
  std::string rv;
  switch(type)
    {
    case fmsynth::NodeFilter::Type::LOW_PASS:  rv = "LowPass";  break;
    case fmsynth::NodeFilter::Type::HIGH_PASS: rv = "HighPass"; break;
    }
  return rv;
}


fmsynth::NodeFilter::Type WidgetNodeFilter::FilterTypeFromString(const std::string & string) const
{
  if(string == "LowPass")  return fmsynth::NodeFilter::Type::LOW_PASS;
  if(string == "HighPass") return fmsynth::NodeFilter::Type::HIGH_PASS;
  assert(false);
  return fmsynth::NodeFilter::Type::LOW_PASS;
}


void WidgetNodeFilter::UpdateFilterType()
{
  int sind;
  switch(_type)
    {
    case fmsynth::NodeFilter::Type::LOW_PASS:  sind = 0; break;
    case fmsynth::NodeFilter::Type::HIGH_PASS: sind = 1; break;
    }
  _ui_node_filter->_type->setCurrentIndex(sind);
  SetNodeType("Filter", "Filter" + FilterTypeString(_type));
}
