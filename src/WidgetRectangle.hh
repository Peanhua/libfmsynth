#ifndef WIDGET_RECTANGLE_HH_
#define WIDGET_RECTANGLE_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "QtIncludeBegin.hh"
#include <QtWidgets/QWidget>
#include <QtGui/QPainterPath>
#include "QtIncludeEnd.hh"


class WidgetRectangle : public QWidget
{
public:
  WidgetRectangle(QWidget * parent, const QPoint & from, const QPoint & to);

  void SetColor(const QColor & color);

  void paintEvent(QPaintEvent * event) override;
  
private:
  QPainterPath _path;
  QSize        _size;
  QColor       _color;
};

#endif
