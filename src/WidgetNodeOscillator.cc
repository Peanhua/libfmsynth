/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeOscillator.hh"
#include "NodeOscillator.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeOscillator.hh"
#include "QtIncludeEnd.hh"


WidgetNodeOscillator::WidgetNodeOscillator(QWidget * parent, NodeOscillator::Type type)
  : WidgetNode(parent, new NodeOscillator, true, true),
    _type(type),
    _node_oscillator(dynamic_cast<NodeOscillator *>(GetNode())),
    _ui_node_oscillator(new Ui::NodeOscillator)
{
  _ui_node->_input_aux->setToolTip("Pulse duty cycle");
  SetConnectorsRanges();

  _ui_node_oscillator->setupUi(_ui_node->_content);

  _ui_node_oscillator->_type->addItem("Sine");
  _ui_node_oscillator->_type->addItem("Triangle");
  _ui_node_oscillator->_type->addItem("Sawtooth");
  _ui_node_oscillator->_type->addItem("Pulse");
  _ui_node_oscillator->_type->addItem("Noise");
  UpdateOscillatorType();

  connect(_ui_node_oscillator->_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
          [this](int index)
          {
            switch(index)
              {
              case 0: _type = NodeOscillator::Type::SINE;     break;
              case 1: _type = NodeOscillator::Type::TRIANGLE; break;
              case 2: _type = NodeOscillator::Type::SAWTOOTH; break;
              case 3: _type = NodeOscillator::Type::PULSE;    break;
              case 4: _type = NodeOscillator::Type::NOISE;    break;
              }
            UpdateOscillatorType();
          });
  
  adjustSize();
  ListenWidgetChanges({
      _ui_node_oscillator->_type,
      _ui_node_oscillator->_pulse_duty_cycle
    });
}


void WidgetNodeOscillator::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_oscillator->SetType(_type);
  _node_oscillator->SetPulseDutyCycle(_ui_node_oscillator->_pulse_duty_cycle->value());
}


void WidgetNodeOscillator::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _type = _node_oscillator->GetType();
  _ui_node_oscillator->_pulse_duty_cycle->setValue(_node_oscillator->GetPulseDutyCycle());
  UpdateOscillatorType();
}


std::string WidgetNodeOscillator::OscillatorTypeString(NodeOscillator::Type type) const
{
  std::string rv;
  switch(type)
    {
    case NodeOscillator::Type::SINE:     rv = "Sine";     break;
    case NodeOscillator::Type::PULSE:    rv = "Pulse";   break;
    case NodeOscillator::Type::TRIANGLE: rv = "Triangle"; break;
    case NodeOscillator::Type::SAWTOOTH: rv = "Sawtooth"; break;
    case NodeOscillator::Type::NOISE:    rv = "Noise";    break;
    }
  return rv;
}


NodeOscillator::Type WidgetNodeOscillator::OscillatorTypeFromString(const std::string & string) const
{
  if(string == "Sine")     return NodeOscillator::Type::SINE;
  if(string == "Triangle") return NodeOscillator::Type::TRIANGLE;
  if(string == "Sawtooth") return NodeOscillator::Type::SAWTOOTH;
  if(string == "Pulse")    return NodeOscillator::Type::PULSE;
  if(string == "Noise")    return NodeOscillator::Type::NOISE;
  assert(false);
  return NodeOscillator::Type::NOISE;
}


void WidgetNodeOscillator::UpdateOscillatorType()
{
  int sind;
  switch(_type)
    {
    case NodeOscillator::Type::SINE:     sind = 0; break;
    case NodeOscillator::Type::TRIANGLE: sind = 1; break;
    case NodeOscillator::Type::SAWTOOTH: sind = 2; break;
    case NodeOscillator::Type::PULSE:    sind = 3; break;
    case NodeOscillator::Type::NOISE:    sind = 4; break;
    }
  _ui_node_oscillator->_type->setCurrentIndex(sind);
  SetNodeType("Oscillator", "Oscillator" + OscillatorTypeString(_type));

  _ui_node_oscillator->_pulse->setVisible(_type == NodeOscillator::Type::PULSE);
  _ui_node->_input_aux->setVisible(_type == NodeOscillator::Type::PULSE);

  adjustSize();
}


void WidgetNodeOscillator::UpdateConnectorStates()
{
  WidgetNode::UpdateConnectorStates();
  bool isconnected = _ui_node->_input_aux->IsConnected();
  _ui_node_oscillator->_pulse_duty_cycle->setEnabled(!isconnected);
}
