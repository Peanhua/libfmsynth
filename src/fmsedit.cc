/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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
#include <filesystem>
#include <iostream>


int main(int argc, char *argv[])
{
  try {
    std::filesystem::current_path(DATADIR);
  } catch([[maybe_unused]] const std::exception & e)
    {
      std::cout << argv[0] << ": Warning, installation directory " << DATADIR << " is not accessible, running from current directory." << std::endl;
    }
  
  Settings settings;
  UserSettings = &settings;
  settings.Load();
  
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
