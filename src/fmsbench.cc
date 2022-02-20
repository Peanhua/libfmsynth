/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Blueprint.hh"
#include "Util.hh"
#include <optional>
#include <cxxopts.hpp>


struct Configuration
{
  std::string  filename;
  double       time;
  unsigned int samples_per_second;
};


std::optional<Configuration> ParseCommandline(int argc, char * argv[])
{
  Configuration rv;
  
  cxxopts::Options options(argv[0], "Benchmark .sbp files.");
  options.custom_help("[OPTION...] <filename>");
  options.add_options()
    ("h,help",               "Print help (this text).")
    ("s,samples-per-second", "Set samples per second.", cxxopts::value<unsigned int>()->default_value("44100"))
    ("t,time",               "Set playback time in seconds.", cxxopts::value<double>()->default_value("300"))
    ;

  auto cmdline = options.parse(argc, argv);
  rv.samples_per_second = cmdline["samples-per-second"].as<unsigned int>();
  rv.time               = cmdline["time"].as<double>();

  if(cmdline.count("help"))
    {
      std::cerr << options.help() << std::endl;
      return std::nullopt;
    }

  if(argc >= 2)
    rv.filename = argv[1];
  
  if(rv.filename.length() == 0)
    {
      std::cerr << argv[0] << ": Error, missing filename.\n";
      std::cerr << options.help() << std::endl;
      return std::nullopt;
    }
  
  return rv;
}


int main(int argc, char * argv[])
{
  auto cmdconf = ParseCommandline(argc, argv);
  if(!cmdconf.has_value())
    return EXIT_FAILURE;
  auto config = cmdconf.value();

  auto [json, error] = fmsynth::util::LoadJsonFile(config.filename);
  if(!json)
    {
      std::cerr << argv[0] << ": " << error << std::endl;
      return EXIT_FAILURE;
    }

  std::chrono::steady_clock clock;
  auto t_start = clock.now();

  fmsynth::Blueprint blueprint;
  auto loadok = blueprint.Load(*json);
  if(!loadok)
    return EXIT_FAILURE;

  auto t_end = clock.now();

  auto t = t_end - t_start;
  std::cout << argv[0] << ": Load '" << config.filename << "': " << std::chrono::duration<double>(t).count() << "s" << std::endl;

  auto totalsamples = static_cast<unsigned int>(config.time * config.samples_per_second);

  t_start = clock.now();
  blueprint.Tick(totalsamples);
  t_end = clock.now();

  t = t_end - t_start;
  std::cout << argv[0] << ": Playback " << config.time << "s (" << totalsamples << " samples): " << std::chrono::duration<double>(t).count() << "s" << std::endl;
  
  return EXIT_SUCCESS;
}
