#ifndef WIDGET_NODE_FILE_OUTPUT_HH_
#define WIDGET_NODE_FILE_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"

namespace fmsynth
{
  class NodeFileOutput;
}


class WidgetNodeFileOutput : public WidgetNode
{
public:
  WidgetNodeFileOutput(QWidget * parent);

  void   NodeToWidget() override;
  void   WidgetToNode() override;

private:
  fmsynth::NodeFileOutput * _node_file_output;
  Ui::NodeFileOutput *      _ui_node_file_output;
};


#endif
