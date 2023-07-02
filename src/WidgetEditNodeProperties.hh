#ifndef WIDGET_EDIT_NODE_PROPERTIES_HH_
#define WIDGET_EDIT_NODE_PROPERTIES_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Ui.hh"
#include "QtIncludeBegin.hh"
#include <QtWidgets/QDialog>
#include "QtIncludeEnd.hh"

class WidgetNode;


class WidgetEditNodeProperties : public QDialog
{
public:
  WidgetEditNodeProperties(QWidget * parent);
  
private:
  Ui::EditNodeProperties * _ui;
  WidgetNode *             _nodewidget;
  
  void                                   ApplyChanges();
  [[nodiscard]] std::vector<std::string> GetExistingIds() const;
};

#endif
