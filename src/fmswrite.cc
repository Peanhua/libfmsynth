/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Blueprint.hh"
#include "NodeAudioDeviceOutput.hh"
#include "Util.hh"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <latch>
#include <optional>
#include <cxxopts.hpp>
#include <AudioFile.h>


struct Configuration
{
  bool         verbose      = false;
  unsigned int samples_per_second;
  std::string  filename;
  std::string  output_filename;
};

static std::optional<Configuration> ParseCommandline(int argc, char * argv[])
{
  Configuration rv;
  
  cxxopts::Options options(argv[0], "Write .sbp file to .wav file.");
  options.custom_help("[OPTION...] <filename>");
  options.add_options()
    ("v,verbose",            "Verbose mode.",           cxxopts::value<bool>()->default_value("false"))
    ("s,samples-per-second", "Set samples per second.", cxxopts::value<unsigned int>()->default_value("44100"))
    ("i,input",              "Input filename.sbp",      cxxopts::value<std::string>())
    ("o,output",             "Output filename.wav",     cxxopts::value<std::string>())
    ("h,help",               "Print help (this text).")
    ;
  options.parse_positional({"input"});
  auto cmdline = options.parse(argc, argv);
  
  rv.verbose            = cmdline["verbose"].as<bool>();
  rv.samples_per_second = cmdline["samples-per-second"].as<unsigned int>();
  
  if(cmdline.count("input") > 0)
    rv.filename         = cmdline["input"].as<std::string>();

  if(cmdline.count("output") > 0)
    rv.output_filename  = cmdline["output"].as<std::string>();
  else
    {
      std::filesystem::path fn{rv.filename};
      fn.replace_extension("wav");
      rv.output_filename = fn.string();
    }
  
  if(cmdline.count("help"))
    {
      std::cerr << options.help() << std::endl;
      return std::nullopt;
    }

  if(rv.filename.empty())
    {
      std::cerr << argv[0] << ": Error, no input file set.\n";
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

  if(config.verbose)
    std::cout << argv[0] << ": Input file '" << config.filename << "'" << std::endl;
      
  auto [json, error] = fmsynth::util::LoadJsonFile(config.filename);
  if(!json)
    {
      std::cerr << error << std::endl;
      return EXIT_FAILURE;
    }

  fmsynth::Blueprint blueprint{};
  bool loadok = blueprint.Load(*json);
  if(!loadok)
    return EXIT_FAILURE;

  // Get list of AudioDeviceOutput nodes, and use the first one to obtain the samples:
  auto ados = blueprint.GetNodesByType("AudioDeviceOutput");
  if(ados.empty())
    {
      std::cerr << argv[0] << ": Error, no AudioDeviceOutput nodes present in the blueprint file '" << config.filename << "'.\n";
      return EXIT_FAILURE;
    }

  blueprint.SetSamplesPerSecond(config.samples_per_second);
      
  if(config.verbose)
    std::cout << argv[0] << ": Output file '" << config.output_filename << "'\n";

  AudioFile<double> output_file{};
  output_file.setSampleRate(config.samples_per_second);

  // Setup callback for the AudioDeviceOutput node to save the sample:
  double current_sample;
  auto node = dynamic_cast<fmsynth::NodeAudioDeviceOutput *>(ados[0]);
  node->SetOnPlaySample(
                       [&current_sample](double sample)
                       {
                         current_sample = sample;
                       }
                       );

  while(!blueprint.IsFinished())
    {
      blueprint.Tick(1); // Calls our callback, storing the sample to 'current_sample'.

      auto ind = output_file.samples[0u].size();
      output_file.setNumSamplesPerChannel(static_cast<int>(ind + 1));
      output_file.samples[0u][ind] = current_sample;
    }

  output_file.save(config.output_filename);
  
  return EXIT_SUCCESS;
}
