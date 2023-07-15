/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>


int main(int argc, char *argv[])
{
  auto cwd = std::filesystem::current_path();
  
  try {
    std::filesystem::current_path(LIBFMSYNTH_DATADIR);
  } catch([[maybe_unused]] const std::exception & e)
    {
      std::cout << argv[0] << ": Warning, installation directory '" << LIBFMSYNTH_DATADIR << "' is not accessible, running from current directory." << std::endl;
    }
  
  cxxopts::Options options(argv[0], "Edit .sbp files.");
  options.custom_help("[OPTION...] [filename]");
  options.add_options()
    ("h,help", "Print help (this text).")
    ;
  
  auto cmdline = options.parse(argc, argv);

  std::filesystem::path path_to_open;
  if(argc >= 2)
    {
      path_to_open = std::filesystem::path(argv[1]);
      if(path_to_open.is_relative())
        path_to_open = cwd / path_to_open;
    }

  if(cmdline.count("help"))
    {
      std::cerr << options.help() << std::endl;
      return EXIT_SUCCESS;
    }

  Settings settings;
  UserSettings = &settings;
  settings.Load();
  
  Player p;
  p.Start();
  p.SetAudioDevice(UserSettings->GetInt("playback_device"));
  
  QApplication app(argc, argv);
  WidgetMainWindow ui(nullptr, path_to_open.string());
  ui.show();
  auto rv = app.exec();
  p.Stop();
  
  settings.Save();
  
  return rv;
}
