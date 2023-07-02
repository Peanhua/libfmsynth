/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeGrowth.hh"
#include "NodeGrowth.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeGrowth.hh"
#include "QtIncludeEnd.hh"


WidgetNodeGrowth::WidgetNodeGrowth(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeGrowth>(), false, true),
    _node_growth(dynamic_cast<fmsynth::NodeGrowth *>(GetNode())),
    _ui_node_growth(new Ui::NodeGrowth)
{
  SetNodeType("Growth");
  _ui_node->_input_amplitude->SetIsOptional();
  _ui_node->_input_form->SetIsOptional();
  SetConnectorsRanges();

  _ui_node_growth->setupUi(_ui_node->_content);
  adjustSize();
  ListenWidgetChanges({
      _ui_node_growth->_growth_formula,
      _ui_node_growth->_end_action
    });
  ListenWidgetChanges(_ui_node_growth->_start_value->GetControlWidgets());
  ListenWidgetChanges(_ui_node_growth->_growth_amount->GetControlWidgets());
  ListenWidgetChanges(_ui_node_growth->_end_value->GetControlWidgets());

  connect(_ui_node_growth->_growth_formula, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]([[maybe_unused]] int index) { UpdateUiVisibility(); });
  connect(_ui_node_growth->_end_action,     QOverload<int>::of(&QComboBox::currentIndexChanged), [this]([[maybe_unused]] int index) { UpdateUiVisibility(); });

  UpdateUiVisibility();
}


void WidgetNodeGrowth::NodeToWidget()
{
  WidgetNode::NodeToWidget();

  _ui_node_growth->_start_value->SetConstantValue(_node_growth->ParamStartValue());
  _ui_node_growth->_growth_formula->setCurrentIndex(static_cast<int>(_node_growth->ParamGrowthFormula()));
  _ui_node_growth->_growth_amount->SetConstantValue(_node_growth->ParamGrowthAmount());
  _ui_node_growth->_end_action->setCurrentIndex(static_cast<int>(_node_growth->ParamEndAction()));
  _ui_node_growth->_end_value->SetConstantValue(_node_growth->ParamEndValue());

  UpdateUiVisibility();
}


void WidgetNodeGrowth::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_growth->ParamStartValue()    = _ui_node_growth->_start_value->GetConstantValue();
  _node_growth->ParamGrowthFormula() = static_cast<fmsynth::NodeGrowth::Formula>(_ui_node_growth->_growth_formula->currentIndex());
  _node_growth->ParamGrowthAmount()  = _ui_node_growth->_growth_amount->GetConstantValue();
  _node_growth->ParamEndAction()     = static_cast<fmsynth::NodeGrowth::EndAction>(_ui_node_growth->_end_action->currentIndex());
  _node_growth->ParamEndValue()      = _ui_node_growth->_end_value->GetConstantValue();
}


void WidgetNodeGrowth::UpdateUiVisibility()
{
  switch(_node_growth->ParamGrowthFormula())
    {
    case fmsynth::NodeGrowth::Formula::Linear:      _ui_node_growth->_growth_amount_label->setText("Growth / s"); break;
    case fmsynth::NodeGrowth::Formula::Logistic:    _ui_node_growth->_growth_amount_label->setText("Maximum");    break;
    case fmsynth::NodeGrowth::Formula::Exponential: _ui_node_growth->_growth_amount_label->setText("Exponent");   break;
    }

  bool end_value_visible = false;
  switch(_node_growth->ParamEndAction())
    {
    case fmsynth::NodeGrowth::EndAction::NoEnd:                                      break;
    case fmsynth::NodeGrowth::EndAction::RepeatLast:       end_value_visible = true; break;
    case fmsynth::NodeGrowth::EndAction::RestartFromStart: end_value_visible = true; break;
    case fmsynth::NodeGrowth::EndAction::Stop:             end_value_visible = true; break;
    }
  _ui_node_growth->_end_value_label->setVisible(end_value_visible);
  _ui_node_growth->_end_value->setVisible(end_value_visible);
}
