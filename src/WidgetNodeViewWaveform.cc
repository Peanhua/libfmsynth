/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeViewWaveform.hh"
#include "NodeMemoryBuffer.hh"
#include "StdFormat.hh"
#include <cassert>
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeViewWaveform.hh"
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDir>
#include "QtIncludeEnd.hh"


WidgetNodeViewWaveform::WidgetNodeViewWaveform(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeMemoryBuffer>(), true, false),
    _node_memorybuffer(dynamic_cast<fmsynth::NodeMemoryBuffer *>(GetNode())),
    _ui_node_view_waveform(new Ui::NodeViewWaveform)
{
  SetNodeType("ViewWaveform");
  const auto range = fmsynth::Input::Range::MinusOne_One;
  _node_memorybuffer->GetInput(fmsynth::Node::Channel::Form)->SetInputRange(range);
  _ui_node->_output_container->setVisible(false);
  _ui_node_view_waveform->setupUi(_ui_node->_content);
  _ui_node_view_waveform->_graph->SetRange(range);
  SetConnectorsRanges();

  ListenWidgetChanges({
      _ui_node_view_waveform->_max_time
    });

  connect(_ui_node_view_waveform->_refresh_graph_button, &QPushButton::clicked,
          [this]([[maybe_unused]] bool checked) {
            _ui_node_view_waveform->_graph->ForceRedraw();
            Redraw();
          });

  startTimer(std::chrono::milliseconds(100));
}


void WidgetNodeViewWaveform::timerEvent([[maybe_unused]] QTimerEvent * event)
{
  Redraw();
}


void WidgetNodeViewWaveform::Redraw()
{
  double length;
  {
    std::lock_guard lock(_node_memorybuffer->GetLockMutex());
    length = static_cast<double>(_node_memorybuffer->GetData().size()) / static_cast<double>(_node_memorybuffer->GetSamplesPerSecond());
  }
  _ui_node_view_waveform->_length_label->setText(QString::fromStdString(format("{:.1f}s", length)));

  _ui_node_view_waveform->_graph->Update(_node_memorybuffer, _ui_node_view_waveform->_max_time->value());
}

void WidgetNodeViewWaveform::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_view_waveform->_graph->ForceRedraw();
}


void WidgetNodeViewWaveform::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  auto node = dynamic_cast<fmsynth::NodeMemoryBuffer *>(GetNode());
  assert(node);
  {
    std::lock_guard lock(node->GetLockMutex());
    node->SetMaxLength(_ui_node_view_waveform->_max_time->value());
  }
  _ui_node_view_waveform->_graph->ForceRedraw();
}


json11::Json WidgetNodeViewWaveform::to_json() const
{
  auto rv = WidgetNode::to_json().object_items();
  rv["viewwaveform_max_length"] = _ui_node_view_waveform->_max_time->value();
  return rv;
}


void WidgetNodeViewWaveform::SetFromJson(const json11::Json & json)
{
  _ui_node_view_waveform->_max_time->setValue(json["viewwaveform_max_length"].number_value());
  WidgetNode::SetFromJson(json);
}
