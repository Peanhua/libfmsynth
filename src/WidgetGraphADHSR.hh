#ifndef WIDGET_GRAPH_ADHSR_HH_
#define WIDGET_GRAPH_ADHSR_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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


class WidgetGraphADHSR : public QWidget
{
public:
  WidgetGraphADHSR(QWidget * parent);
  void SetEnvelope(double attack, double decay, double hold, double sustain, double release);
  
  void resizeEvent(QResizeEvent * event) override;
  void paintEvent(QPaintEvent * event) override;
  
private:
  QPainterPath _path;
  double _attack;
  double _decay;
  double _hold;
  double _sustain;
  double _release;
};

#endif
