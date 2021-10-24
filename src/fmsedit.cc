/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeConstant.hh"
#include "NodeADHSR.hh"
#include "NodeFileOutput.hh"
#include "NodeOscillator.hh"
#include "Player.hh"
#include "Settings.hh"
#include "WidgetMainWindow.hh"
#include "QtIncludeBegin.hh"
#include <QtWidgets/QApplication>
#include "QtIncludeEnd.hh"
#include <alut.h>
#include <iostream>


int main(int argc, char *argv[])
{
  Settings settings;
  UserSettings = &settings;
  settings.Load();
  
  auto success = alutInit(nullptr, nullptr);
  if(!success)
    return EXIT_FAILURE;
  
  Player p;
  p.Start();
  
  QApplication app(argc, argv);
  WidgetMainWindow ui;
  ui.show();
  auto rv = app.exec();
  p.Stop();
  
  settings.Save();
  
  return rv;
}
