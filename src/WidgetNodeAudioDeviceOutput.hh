#ifndef WIDGET_NODE_AUDIO_DEVICE_OUTPUT_HH_
#define WIDGET_NODE_AUDIO_DEVICE_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"

class NodeAudioDeviceOutput;


class WidgetNodeAudioDeviceOutput : public WidgetNode
{
public:
  WidgetNodeAudioDeviceOutput(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;

private:
  NodeAudioDeviceOutput *     _node_audio_device_output;
  Ui::NodeAudioDeviceOutput * _ui_node_audio_device_output;

  void UpdateMuteButton();
};


#endif
