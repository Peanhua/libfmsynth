/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetAddNode.hh"
#include "Ui.hh"
#include "QtIncludeBegin.hh"
#include <QtCore/QMimeData>
#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>
#include "QtIncludeEnd.hh"


WidgetAddNode::WidgetAddNode(QWidget * parent)
  : QPushButton(parent)
{
}

void WidgetAddNode::mousePressEvent(QMouseEvent * event)
{
  if(event->button() == Qt::LeftButton)
    {
      auto mime = new QMimeData();
      mime->setText("New:" + property("NodeType").toString());
      
      auto drag = new QDrag(this);
      drag->setMimeData(mime);
      drag->setPixmap(icon().pixmap(32, 32));
      drag->exec();
    }
  QPushButton::mousePressEvent(event);
}
