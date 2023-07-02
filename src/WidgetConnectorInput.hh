#ifndef WIDGET_CONNECTOR_INPUT_HH_
#define WIDGET_CONNECTOR_INPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetConnector.hh"

class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;
class QMouseEvent;
class WidgetBlueprint;
class WidgetNode;


class WidgetConnectorInput : public WidgetConnector
{
public:
  WidgetConnectorInput(QWidget * parent);
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event)           override;
  void mousePressEvent(QMouseEvent * event)   override;

private:
  [[nodiscard]] WidgetBlueprint * GetBlueprint()                const;
  [[nodiscard]] WidgetNode *      GetNode(QWidget * for_widget) const;
};

#endif
