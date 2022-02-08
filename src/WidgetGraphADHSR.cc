/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetGraphADHSR.hh"
#include "QtIncludeBegin.hh"
#include <QtGui/QPainter>
#include "QtIncludeBegin.hh"


WidgetGraphADHSR::WidgetGraphADHSR(QWidget * parent)
  : QWidget(parent),
    _attack(0),
    _decay(0),
    _hold(0),
    _sustain(0),
    _release(0)
{
}


void WidgetGraphADHSR::SetEnvelope(double attack, double decay, double hold, double sustain, double release)
{
  _attack  = attack;
  _decay   = decay;
  _hold    = hold;
  _sustain = sustain;
  _release = release;

  double width = size().width();
  double height = size().height();
  double totaltime = attack + decay + hold + release;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
  _path.clear();
#else
  _path = QPainterPath();
#endif
  _path.moveTo(QPoint(0,                                                             static_cast<int>(height - 1)));
  _path.lineTo(QPoint(static_cast<int>(width * attack / totaltime),                  0));
  _path.lineTo(QPoint(static_cast<int>(width * (attack + decay) / totaltime),        static_cast<int>((1.0 - sustain) * height)));
  _path.lineTo(QPoint(static_cast<int>(width * (attack + decay + hold) / totaltime), static_cast<int>((1.0 - sustain) * height)));
  _path.lineTo(QPoint(static_cast<int>(width - 1),                                   static_cast<int>(height - 1)));

  update();
}


void WidgetGraphADHSR::resizeEvent([[maybe_unused]] QResizeEvent *event)
{
  SetEnvelope(_attack, _decay, _hold, _sustain, _release);
}


void WidgetGraphADHSR::paintEvent([[maybe_unused]] QPaintEvent * event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QColor color(0, 100, 255);
  painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawPath(_path);
}
