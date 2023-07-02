#ifndef WIDGET_ADD_NODE_HH_
#define WIDGET_ADD_NODE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "QtIncludeBegin.hh"
#include <QtWidgets/QPushButton>
#include "QtIncludeEnd.hh"


class WidgetAddNode : public QPushButton
{
public:
  WidgetAddNode(QWidget * parent = nullptr);

  void mousePressEvent(QMouseEvent * event) override;
};

#endif
