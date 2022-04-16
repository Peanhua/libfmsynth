/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "AudioDevice.hh"
#include "Blueprint.hh"
#include "NodeAudioDeviceOutput.hh"
#include "Util.hh"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <latch>
#include <optional>
#include <cxxopts.hpp>
#include <AudioFile.h>


struct Configuration
{
  bool                verbose  = false;
  unsigned int        samples_per_second;
  std::string         filename;
  std::string         output_filename;
  AudioFile<double> * output_file = nullptr;
};

std::optional<Configuration> ParseCommandline(int argc, char * argv[])
{
  Configuration rv;
  
  cxxopts::Options options(argv[0], "Play .sbp files.");
  options.custom_help("[OPTION...] <filename>");
  options.add_options()
    ("v,verbose",            "Verbose mode.",                                                    cxxopts::value<bool>()->default_value("false"))
    ("s,samples-per-second", std::string("Set samples per second. Use 0 for maximum possible."), cxxopts::value<unsigned int>()->default_value("44100"))
    ("o,output",             "Write to file in WAV format.",                                     cxxopts::value<std::string>())
    ("h,help",               "Print help (this text).")
    ;
  
  auto cmdline = options.parse(argc, argv);
  
  rv.verbose = cmdline["verbose"].as<bool>();
  rv.samples_per_second = cmdline["samples-per-second"].as<unsigned int>();

  if(cmdline.count("output") > 0)
    rv.output_filename = cmdline["output"].as<std::string>();

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



static std::optional<unsigned int> GetSampleRate(unsigned int request_rate, const std::vector<unsigned int> & sample_rates)
{
  assert(sample_rates.size() > 0);
  
  if(request_rate == 0)
    return *std::max_element(sample_rates.cbegin(), sample_rates.cend());

  auto supported = std::find(sample_rates.cbegin(), sample_rates.cend(), request_rate) != sample_rates.cend();
  if(!supported)
    return std::nullopt;

  return request_rate;
}



int main(int argc, char * argv[])
{
  auto cmdconf = ParseCommandline(argc, argv);
  if(!cmdconf.has_value())
    return EXIT_FAILURE;
  auto config = cmdconf.value();

  if(config.verbose)
    std::cout << argv[0] << ": Input file             = '" << config.filename << "'" << std::endl;

  auto [json, error] = fmsynth::util::LoadJsonFile(config.filename);
  if(!json)
    {
      std::cerr << error << std::endl;
      return EXIT_FAILURE;
    }

  auto bp = std::make_shared<fmsynth::Blueprint>();
  auto blueprint = bp.get();
  auto loadok = blueprint->Load(*json);
  if(!loadok)
    return EXIT_FAILURE;

  AudioDevice adev;

  auto sr = GetSampleRate(config.samples_per_second, adev.GetSampleRates());
  if(!sr.has_value())
    {
      std::cerr << argv[0] << ": Error, no suitable sample rate available." << std::endl;
      return EXIT_FAILURE;
    }
  auto sample_rate = sr.value();
  blueprint->SetSamplesPerSecond(sample_rate);

  if(config.output_filename.length() > 0)
    {
      config.output_file = new AudioFile<double>();
      assert(config.output_file);
      config.output_file->setSampleRate(config.samples_per_second);
    }
  
  if(config.verbose)
    {
      std::cout << argv[0] << ": Audio device           = " << adev.GetDeviceName() << "\n";
      {
        std::cout << argv[0] << ": Available sample rates = ";
        bool first = true;
        for(auto rate : adev.GetSampleRates())
          {
            if(!first)
              std::cout << ", ";
            first = false;
            
            std::cout << rate;
          }
        std::cout << "\n";
      }
      std::cout << argv[0] << ": Sample rate            = " << sample_rate << "\n";
      if(config.output_filename.length() > 0)
        std::cout << argv[0] << ": Output file            = '" << config.output_filename << "'\n";
      
      std::cout << std::flush;
    }

  
  std::latch done{1};

  adev.SetOnPostTick([&config, &adev, &done](double sample)
  {
    if(config.output_file)
      {
        auto ind = config.output_file->samples[0u].size();
        config.output_file->setNumSamplesPerChannel(static_cast<int>(ind + 1));
        config.output_file->samples[0u][ind] = sample;
      }
  
    if(adev.GetBlueprint()->IsFinished())
      done.count_down();
  });
  
  adev.Play(bp);

  if(config.verbose)
    {
      std::cout << argv[0] << ": Audio output nodes     = " << adev.GetInputNodes().size() << " : ";
      bool first = true;
      for(auto n : adev.GetInputNodes())
        {
          if(!first)
            std::cout << ", ";
          first = false;
          std::cout << "'" << n->GetId() << "'";
        }
      std::cout << std::endl;
    }
  
  done.wait();
  
  if(config.output_file)
    config.output_file->save(config.output_filename);

  return EXIT_SUCCESS;
}
