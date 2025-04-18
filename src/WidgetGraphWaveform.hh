#ifndef WIDGET_GRAPH_WAVEFORM_HH_
#define WIDGET_GRAPH_WAVEFORM_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Input.hh"
#include "QtIncludeBegin.hh"
#include <QtWidgets/QWidget>
#include <QtGui/QPainterPath>
#include "QtIncludeEnd.hh"

namespace fmsynth
{
  class NodeMemoryBuffer;
}


class WidgetGraphWaveform : public QWidget
{
public:
  WidgetGraphWaveform(QWidget * parent);

  void SetRange(fmsynth::Input::Range range);
  void ForceRedraw();
  void Update(fmsynth::NodeMemoryBuffer * buffer, double length);
  
  void paintEvent(QPaintEvent * event) override;
  
private:
  QPainterPath          _path;
  fmsynth::Input::Range _range;
  unsigned int          _last_draw_index;
  bool                  _force_redraw;
  std::vector<double>   _waveform;

  [[nodiscard]] double Normalize(double value) const;
};

#endif
