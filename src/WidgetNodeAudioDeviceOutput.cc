/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeAudioDeviceOutput.hh"
#include "NodeAudioDeviceOutput.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeAudioDeviceOutput.hh"
#include "QtIncludeEnd.hh"


WidgetNodeAudioDeviceOutput::WidgetNodeAudioDeviceOutput(QWidget * parent)
  : WidgetNode(parent, std::make_shared<fmsynth::NodeAudioDeviceOutput>(), true, false),
    _node_audio_device_output(dynamic_cast<fmsynth::NodeAudioDeviceOutput *>(GetNode())),
    _ui_node_audio_device_output(new Ui::NodeAudioDeviceOutput)
{
  SetNodeType("AudioDeviceOutput");
  _ui_node->_title->setIcon(QIcon::fromTheme("audio-card"));
  _ui_node->_title->setText("Audio device");
  SetConnectorsRanges();
  _ui_node_audio_device_output->setupUi(_ui_node->_content);
  adjustSize();

  connect(_ui_node_audio_device_output->_mute, &QPushButton::clicked,
          [this]([[maybe_unused]] bool checked)
          {
            auto muted = _node_audio_device_output->IsMuted();
            _node_audio_device_output->SetMuted(!muted);
            UpdateMuteButton();
          }
          );
  ListenWidgetChanges({
      _ui_node_audio_device_output->_volume,
      _ui_node_audio_device_output->_mute
    });
}


void WidgetNodeAudioDeviceOutput::UpdateMuteButton()
{
  QIcon icon;
  if(_node_audio_device_output->IsMuted())
    icon = QIcon::fromTheme("audio-volume-muted");
  else
    icon = QIcon::fromTheme("audio-volume-high");
  _ui_node_audio_device_output->_mute->setIcon(icon);
}


void WidgetNodeAudioDeviceOutput::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_audio_device_output->_volume->setValue(static_cast<int>(_node_audio_device_output->GetVolume() * 100.0));
  UpdateMuteButton();
}


void WidgetNodeAudioDeviceOutput::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_audio_device_output->SetVolume(static_cast<double>(_ui_node_audio_device_output->_volume->value()) / 100.0);
}
