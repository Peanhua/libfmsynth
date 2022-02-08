/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeADHSR.hh"
#include "NodeADHSR.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeADHSR.hh"
#include "QtIncludeEnd.hh"
#include <utility>
#include <vector>


WidgetNodeADHSR::WidgetNodeADHSR(QWidget * parent)
  : WidgetNode(parent, new fmsynth::NodeADHSR, true, true),
    _node_adhsr(dynamic_cast<fmsynth::NodeADHSR *>(GetNode())),
    _ui_node_adhsr(new Ui::NodeADHSR)
{
  SetNodeType("ADHSR", "ADSR");
  _ui_node->_title->setText("ADHSR envelope");
  SetConnectorsRanges();
  _ui_node_adhsr->setupUi(_ui_node->_content);
  _ui_node_adhsr->_end_action->addItem("Stop");
  _ui_node_adhsr->_end_action->addItem("Restart");
  _ui_node_adhsr->_end_action->addItem("Do nothing");
  adjustSize();
  SetSlidersFromSpinboxes();

  connect(_ui_node_adhsr->_max_time_range, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSlidersFromSpinboxes(); });

  connect(_ui_node_adhsr->_attack_slider,  &QSlider::valueChanged, [this]([[maybe_unused]] int value) { SetSpinboxFromSlider(_ui_node_adhsr->_attack,  _ui_node_adhsr->_attack_slider);  });
  connect(_ui_node_adhsr->_decay_slider,   &QSlider::valueChanged, [this]([[maybe_unused]] int value) { SetSpinboxFromSlider(_ui_node_adhsr->_decay,   _ui_node_adhsr->_decay_slider);   });
  connect(_ui_node_adhsr->_hold_slider,    &QSlider::valueChanged, [this]([[maybe_unused]] int value) { SetSpinboxFromSlider(_ui_node_adhsr->_hold,    _ui_node_adhsr->_hold_slider);    });
  connect(_ui_node_adhsr->_sustain_slider, &QSlider::valueChanged, [this]([[maybe_unused]] int value) { SetSpinboxFromSlider(_ui_node_adhsr->_sustain, _ui_node_adhsr->_sustain_slider); });
  connect(_ui_node_adhsr->_release_slider, &QSlider::valueChanged, [this]([[maybe_unused]] int value) { SetSpinboxFromSlider(_ui_node_adhsr->_release, _ui_node_adhsr->_release_slider); });

  connect(_ui_node_adhsr->_attack,  QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSliderFromSpinbox(_ui_node_adhsr->_attack,  _ui_node_adhsr->_attack_slider);  });
  connect(_ui_node_adhsr->_decay,   QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSliderFromSpinbox(_ui_node_adhsr->_decay,   _ui_node_adhsr->_decay_slider);   });
  connect(_ui_node_adhsr->_hold,    QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSliderFromSpinbox(_ui_node_adhsr->_hold,    _ui_node_adhsr->_hold_slider);    });
  connect(_ui_node_adhsr->_sustain, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSliderFromSpinbox(_ui_node_adhsr->_sustain, _ui_node_adhsr->_sustain_slider); });
  connect(_ui_node_adhsr->_release, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double value) { SetSliderFromSpinbox(_ui_node_adhsr->_release, _ui_node_adhsr->_release_slider); });

  UpdateGraph();
  ListenWidgetChanges({
      _ui_node_adhsr->_attack,
      _ui_node_adhsr->_decay,
      _ui_node_adhsr->_hold,
      _ui_node_adhsr->_sustain,
      _ui_node_adhsr->_release,
      _ui_node_adhsr->_max_time_range,
      _ui_node_adhsr->_end_action
    });
}


void WidgetNodeADHSR::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_adhsr->_attack->setValue(           _node_adhsr->GetAttackTime());
  _ui_node_adhsr->_decay->setValue(            _node_adhsr->GetDecayTime());
  _ui_node_adhsr->_hold->setValue(             _node_adhsr->GetHoldTime());
  _ui_node_adhsr->_sustain->setValue(          _node_adhsr->GetSustainLevel());
  _ui_node_adhsr->_release->setValue(          _node_adhsr->GetReleaseTime());
  _ui_node_adhsr->_end_action->setCurrentIndex(static_cast<int>(_node_adhsr->GetEndAction()));
}


void WidgetNodeADHSR::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_adhsr->Set(_ui_node_adhsr->_attack->value(),
                   _ui_node_adhsr->_decay->value(),
                   _ui_node_adhsr->_hold->value(),
                   _ui_node_adhsr->_sustain->value(),
                   _ui_node_adhsr->_release->value(),
                   static_cast<fmsynth::NodeADHSR::EndAction>(_ui_node_adhsr->_end_action->currentIndex())
                   );
}


void WidgetNodeADHSR::SetSlidersFromSpinboxes()
{
  std::vector<std::pair<QDoubleSpinBox *, QSlider *>> controls
    {
      { _ui_node_adhsr->_attack,  _ui_node_adhsr->_attack_slider },
      { _ui_node_adhsr->_decay,   _ui_node_adhsr->_decay_slider },
      { _ui_node_adhsr->_hold,    _ui_node_adhsr->_hold_slider },
      { _ui_node_adhsr->_sustain, _ui_node_adhsr->_sustain_slider },
      { _ui_node_adhsr->_release, _ui_node_adhsr->_release_slider }
    };
  for(auto [spinbox, slider] : controls)
    SetSliderFromSpinbox(spinbox, slider);
}


void WidgetNodeADHSR::SetSpinboxesFromSliders()
{
  std::vector<std::pair<QDoubleSpinBox *, QSlider *>> controls
    {
      { _ui_node_adhsr->_attack,  _ui_node_adhsr->_attack_slider },
      { _ui_node_adhsr->_decay,   _ui_node_adhsr->_decay_slider },
      { _ui_node_adhsr->_hold,    _ui_node_adhsr->_hold_slider },
      { _ui_node_adhsr->_sustain, _ui_node_adhsr->_sustain_slider },
      { _ui_node_adhsr->_release, _ui_node_adhsr->_release_slider } 
    };
  for(auto [spinbox, slider] : controls)
    SetSpinboxFromSlider(spinbox, slider);
}


void WidgetNodeADHSR::SetSliderFromSpinbox(QDoubleSpinBox * spinbox, QSlider * slider)
{
  double maxrange;
  if(spinbox == _ui_node_adhsr->_sustain)
    maxrange = 1;
  else
    maxrange = _ui_node_adhsr->_max_time_range->value();
  auto v = spinbox->value() * 1000.0 / maxrange;
  slider->setValue(static_cast<int>(v));

  UpdateGraph();
}


void WidgetNodeADHSR::SetSpinboxFromSlider(QDoubleSpinBox * spinbox, QSlider * slider)
{
  double maxrange;
  if(spinbox == _ui_node_adhsr->_sustain)
    maxrange = 1;
  else
    maxrange = _ui_node_adhsr->_max_time_range->value();
  auto v = static_cast<double>(slider->value()) / 1000.0 * maxrange;
  spinbox->setValue(v);

  UpdateGraph();
}


json11::Json WidgetNodeADHSR::to_json() const
{
  auto rv = WidgetNode::to_json().object_items();
  rv["adhsr_max_time_range"] = _ui_node_adhsr->_max_time_range->value();
  return rv;
}


void WidgetNodeADHSR::SetFromJson(const json11::Json & json)
{
  _ui_node_adhsr->_max_time_range->setValue(json["adhsr_max_time_range"].number_value());
  WidgetNode::SetFromJson(json);
}


void WidgetNodeADHSR::UpdateGraph()
{
  _ui_node_adhsr->_graph->SetEnvelope(_ui_node_adhsr->_attack->value(),
                                      _ui_node_adhsr->_decay->value(),
                                      _ui_node_adhsr->_hold->value(),
                                      _ui_node_adhsr->_sustain->value(),
                                      _ui_node_adhsr->_release->value()
                                      );
}

