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
#include "NodeAudioDeviceOutput.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <latch>
#include <optional>
#include <thread>
#include <utility>
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wold-style-cast"
# pragma GCC diagnostic ignored "-Wdeprecated-copy"
# pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <RtAudio.h>
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
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


static std::optional<std::string> LoadText(const std::string & filename)
{
  std::string text;
  std::ifstream fp(filename);
  if(!fp)
    {
      std::cerr << "Failed to open '" + filename + "' for reading.\n";
      return std::nullopt;
    }
  
  std::string tmp;
  while(std::getline(fp, tmp))
    text += tmp + '\n';
      
  // todo: Add proper error message.
  if(!fp.eof())
    {
      std::cerr << "Error loading '" + filename + "': something went wrong\n";
      return std::nullopt;
    }
  
  return text;
}

static json11::Json * LoadJson(const std::string & json_string)
{
  auto json = new json11::Json();
  if(!json_string.empty())
    {
      std::string err;
      *json = json11::Json::parse(json_string, err);
      if(!json->is_object())
        {
          std::cerr << "Error while parsing json: " << err << std::endl;
          delete json;
          json = nullptr;
        }
    }
  return json;
}


static std::latch done{1};

static int playback(void *                  outputBuffer,
                    [[maybe_unused]] void * inputBuffer,
                    unsigned int            nBufferFrames,
                    [[maybe_unused]] double streamTime,
                    RtAudioStreamStatus     status,
                    void *                  userData)
{
  assert(!status);
  
  auto [blueprint, audiodevicenodes, output_file] = *static_cast<std::tuple<fmsynth::Blueprint *, std::vector<fmsynth::NodeAudioDeviceOutput *> *, AudioFile<double> *> *>(userData);
  assert(blueprint);
  assert(audiodevicenodes->size() > 0);
  
  std::lock_guard lock(blueprint->GetLockMutex());
  
  double currentsample = 0;
  for(auto n : *audiodevicenodes)
    n->SetCallbacks(
                    [&currentsample]([[maybe_unused]] fmsynth::NodeAudioDeviceOutput * node, double sample)
                    {
                      currentsample += sample;
                    },
                    []([[maybe_unused]] fmsynth::NodeAudioDeviceOutput * node)
                    {
                    }
                    );
  
  
  auto buffer = static_cast<double *>(outputBuffer);
  for(unsigned i = 0; i < nBufferFrames; i++)
    {
      currentsample = 0;
      blueprint->Tick(1);
      *buffer++ = currentsample;
      if(output_file)
        {
          auto ind = output_file->samples[0u].size();
          output_file->setNumSamplesPerChannel(static_cast<int>(ind + 1));
          output_file->samples[0u][ind] = currentsample;
        }
    }
  
  if(blueprint->IsFinished())
    {
      done.count_down();
      return 1;
    }
  
  return 0;
}


static RtAudio * GetDAC()
{
  auto getit = [](RtAudio::Api api) -> RtAudio *
  {
    try
      {
        RtAudio * rv;
        if(api == RtAudio::Api::UNSPECIFIED)
          rv = new RtAudio();
        else
          rv = new RtAudio(api);
        if(rv)
          {
            if((api == RtAudio::Api::UNSPECIFIED || api == rv->getCurrentApi()) && rv->getDeviceCount() > 0)
              return rv;
            else
              delete rv;
          }
      }
    catch(RtAudioError & e)
      {
      }

    return nullptr;
  };

  
  std::vector<RtAudio::Api> apis
    {
#ifdef __linux__
      RtAudio::Api::LINUX_PULSE,
#endif
      RtAudio::Api::UNSPECIFIED
    };
  for(auto api : apis)
    {
      auto rv = getit(api);
      if(rv)
        return rv;
    }

  assert(false);
  return nullptr;
}


static std::optional<unsigned int> GetSampleRate(unsigned int request_rate, const RtAudio::DeviceInfo & deviceinfo)
{
  unsigned int sample_rate = request_rate;
  // No automatic sample rate conversion, change the sample rate to one supported:
  assert(deviceinfo.sampleRates.size() > 0);
  auto r = std::find(deviceinfo.sampleRates.cbegin(), deviceinfo.sampleRates.cend(), sample_rate);

  auto supported = r != deviceinfo.sampleRates.cend();
  
  if(!supported)
    {
      if(request_rate == 0)
        sample_rate = deviceinfo.sampleRates.back();
      else
        return std::nullopt;
    }

  return sample_rate;
}



int main(int argc, char * argv[])
{
  auto cmdconf = ParseCommandline(argc, argv);
  if(!cmdconf.has_value())
    return EXIT_FAILURE;
  auto config = cmdconf.value();

  if(config.verbose)
    std::cout << argv[0] << ": Input file             = '" << config.filename << "'" << std::endl;
  
  auto text = LoadText(config.filename);
  if(!text.has_value())
    return EXIT_FAILURE;

  auto json = LoadJson(text.value());
  if(!json)
    return EXIT_FAILURE;

  fmsynth::Blueprint blueprint;
  auto loadok = blueprint.Load(*json);
  if(!loadok)
    return EXIT_FAILURE;

  auto dac = GetDAC();
  if(!dac)
    {
      std::cerr << argv[0] << ": Error, no audio devices found.\n";
      return EXIT_FAILURE;
    }
  
  RtAudio::StreamParameters parameters;
  parameters.nChannels = 1;
  parameters.firstChannel = 0;
  parameters.deviceId = dac->getDefaultOutputDevice();
  // Try to fix case when dac->getDefaultOutputDevice() sometimes incorrectly returns 0:
  for(unsigned int i = 0; parameters.deviceId == 0 && i < dac->getDeviceCount(); i++)
    try
      {
        auto info = dac->getDeviceInfo(i);
        if(info.outputChannels > 0)
          parameters.deviceId = i;
      }
    catch(const std::exception & e)
      {
      }

  auto deviceinfo = dac->getDeviceInfo(parameters.deviceId);
  if(config.verbose)
    {
      std::cout << argv[0] << ": Audio device           = " << deviceinfo.name << "\n";
      std::cout << argv[0] << ": Available sample rates = ";
      bool first = true;
      for(auto sr : deviceinfo.sampleRates)
        {
          if(!first)
            std::cout << ", ";
          first = false;

          std::cout << sr;
        }
      std::cout << std::endl;
    }
  
  auto sr = GetSampleRate(config.samples_per_second, deviceinfo);
  if(!sr.has_value())
    {
      std::cerr << argv[0] << ": Error, no suitable sample rate available." << std::endl;
      delete dac;
      return EXIT_FAILURE;
    }
  auto sample_rate = sr.value();
  blueprint.SetSamplesPerSecond(sample_rate);

  auto ados = blueprint.GetNodesByType("AudioDeviceOutput");
  if(ados.size() == 0)
    {
      std::cerr << argv[0] << ": Error, no AudioDeviceOutput node in the input.\n";
      delete dac;
      return EXIT_FAILURE;
    }
  std::vector<fmsynth::NodeAudioDeviceOutput *> nodes;
  for(auto n : ados)
    nodes.push_back(dynamic_cast<fmsynth::NodeAudioDeviceOutput *>(n));
  if(config.verbose)
    {
      std::cout << argv[0] << ": Output nodes           = " << nodes.size() << " : ";
      bool first = true;
      for(auto n : nodes)
        {
          if(!first)
            std::cout << ", ";
          first = false;
          std::cout << "'" << n->GetId() << "'";
        }
      std::cout << std::endl;
    }

  if(config.output_filename.length() > 0)
    {
      config.output_file = new AudioFile<double>();
      assert(config.output_file);
      config.output_file->setSampleRate(config.samples_per_second);
    }

  auto callbackdata = std::make_tuple(&blueprint, &nodes, config.output_file);


  if(config.verbose)
    {
      std::cout << argv[0] << ": Sample rate            = " << sample_rate << "\n";
      if(config.output_filename.length() > 0)
        std::cout << argv[0] << ": Output file            = '" << config.output_filename << "'\n";
      std::cout << std::flush;
    }
  
  int rv = EXIT_SUCCESS;
  try
    {
      unsigned int bframes = 1024;
      dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT64, sample_rate, &bframes, playback, &callbackdata);
      dac->startStream();
      done.wait();
    }
  catch(RtAudioError & e)
    {
      e.printMessage();
      rv = EXIT_FAILURE;
    }

  delete dac;
  
  if(config.output_file)
    config.output_file->save(config.output_filename);
  
  return rv;
}
