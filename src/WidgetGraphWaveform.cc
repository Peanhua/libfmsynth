/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetGraphWaveform.hh"
#include "NodeMemoryBuffer.hh"
#include "QtIncludeBegin.hh"
#include <QtGui/QPainter>
#include "QtIncludeBegin.hh"


WidgetGraphWaveform::WidgetGraphWaveform(QWidget * parent)
  : QWidget(parent),
    _range(fmsynth::Input::Range::MinusOne_One),
    _last_draw_index(0),
    _force_redraw(false)
{
}


void WidgetGraphWaveform::ForceRedraw()
{
  _force_redraw = true;
}


void WidgetGraphWaveform::SetRange(fmsynth::Input::Range range)
{
  _range = range;
}


double WidgetGraphWaveform::Normalize(double value) const
{
  switch(_range)
    {
    case fmsynth::Input::Range::Zero_One:     return (value - 0.5) * 2.0;
    case fmsynth::Input::Range::MinusOne_One: return value;
    case fmsynth::Input::Range::Inf_Inf:      return 0;
    }
  assert(false);
  return 0;
}


void WidgetGraphWaveform::Update(fmsynth::NodeMemoryBuffer * buffer, double length)
{
  {
    std::lock_guard lock(buffer->GetLockMutex());
    
    auto & wav = buffer->GetData();
    if(wav.size() > 0)
      {
        double width = size().width();
        double height = (size().height() - 1.0) / 2.0;
        double centery = height;

        if(_force_redraw || _last_draw_index > wav.size())
          {
            _force_redraw    = false;
            _last_draw_index = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
            _path.clear();
#else
            _path = QPainterPath();
#endif
            _path.moveTo(QPoint(-1, static_cast<int>(centery + Normalize(wav[0]) * height)));
          }

        auto samples_per_second = static_cast<double>(buffer->GetSamplesPerSecond());
        while(_last_draw_index < wav.size() && static_cast<double>(_last_draw_index) / samples_per_second < length)
          {
            double x = static_cast<double>(_last_draw_index) / (length * samples_per_second) * width;
            double y = centery + Normalize(wav[_last_draw_index]) * -1.0 * height;
            _path.lineTo(QPoint(static_cast<int>(x), static_cast<int>(y)));
            _last_draw_index++;
          }
      }
  }

  update();
}


void WidgetGraphWaveform::paintEvent([[maybe_unused]] QPaintEvent * event)
{
  QPainter painter(this);
  QColor color(0, 255, 0);
  painter.setPen(QPen(color, 0.25, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawPath(_path);
}
