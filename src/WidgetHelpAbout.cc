/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetHelpAbout.hh"
#include "QtIncludeBegin.hh"
#include "UiHelpAbout.hh"
#include "QtIncludeEnd.hh"


WidgetHelpAbout::WidgetHelpAbout(QWidget * parent)
  : QDialog(parent),
    _ui(new Ui::HelpAbout)
{
  _ui->setupUi(this);
}
