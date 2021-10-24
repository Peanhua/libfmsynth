/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetLine.hh"
#include "QtIncludeBegin.hh"
#include <QtGui/QPainter>
#include "QtIncludeBegin.hh"

WidgetLine::WidgetLine(QWidget * parent, const QPoint & from, const QPoint & to)
  : QWidget(parent)
{
  QPoint topleft(    std::min(from.x(), to.x()), std::min(from.y(), to.y()));
  QPoint bottomright(std::max(from.x(), to.x()), std::max(from.y(), to.y()));
  move(topleft);
  _size.setWidth( std::max(1, bottomright.x() - topleft.x()));
  _size.setHeight(std::max(1, bottomright.y() - topleft.y()));
  resize(_size);

  _path.moveTo(from - topleft);
  _path.lineTo(to - topleft);
}


void WidgetLine::paintEvent([[maybe_unused]] QPaintEvent * event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QColor color("green");
  painter.setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawPath(_path);
}
