/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetConnectorOutput.hh"
#include "QtIncludeBegin.hh"
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include "QtIncludeEnd.hh"


WidgetConnectorOutput::WidgetConnectorOutput(QWidget * parent)
  : WidgetConnector(parent)
{
  _is_input = false;
  SetIsConnected(false);
}


void WidgetConnectorOutput::mousePressEvent(QMouseEvent * event)
{
  if(event->button() == Qt::LeftButton)
    {
      auto drag = new QDrag(this);
      auto mime = new QMimeData();
      
      mime->setText("OutputFrom:" + property("NodeId").toString());
      drag->setMimeData(mime);
      drag->setPixmap(icon().pixmap(16, 16));
      drag->exec();
    }
  QPushButton::mousePressEvent(event);
}
