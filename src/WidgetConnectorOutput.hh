#ifndef WIDGET_CONNECTOR_OUTPUT_HH_
#define WIDGET_CONNECTOR_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetConnector.hh"
#include "QtIncludeBegin.hh"
#include <QtGui/QMouseEvent>
#include "QtIncludeEnd.hh"


class WidgetConnectorOutput : public WidgetConnector
{
public:
  WidgetConnectorOutput(QWidget * parent);
  void mousePressEvent(QMouseEvent * event) override;
};

#endif
