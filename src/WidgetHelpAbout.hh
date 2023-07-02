#ifndef WIDGET_HELP_ABOUT_HH_
#define WIDGET_HELP_ABOUT_HH_
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


class WidgetHelpAbout : public QDialog
{
public:
  WidgetHelpAbout(QWidget * parent);
  
private:
  Ui::HelpAbout * _ui;
};

#endif
