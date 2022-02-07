/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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


static std::string LoadText(const std::string & filename)
{
  std::string text;
  std::ifstream fp(filename);
  if(fp)
    {
      std::string tmp;
      while(std::getline(fp, tmp))
        text += tmp + '\n';
      
      // todo: Add proper error message.
      if(!fp.eof())
        std::cerr << "Error loading '" + filename + "': something went wrong\n";
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
  
  auto [blueprint, audiodevicenode] = *static_cast<std::pair<Blueprint *, NodeAudioDeviceOutput *> *>(userData);
  assert(blueprint);
  assert(audiodevicenode);
  
  std::lock_guard lock(blueprint->GetLockMutex());
  
  double currentsample = 0;
  audiodevicenode->SetCallbacks(
                                [&currentsample]([[maybe_unused]] NodeAudioDeviceOutput * node, double sample)
                                {
                                  currentsample = sample;
                                },
                                []([[maybe_unused]] NodeAudioDeviceOutput * node)
                                {
                                }
                                );
  
  
  auto buffer = static_cast<double *>(outputBuffer);
  for(unsigned i = 0; i < nBufferFrames; i++)
    {
      blueprint->Tick(1);
      *buffer++ = currentsample;
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


static unsigned int GetSampleRate(const RtAudio::DeviceInfo & deviceinfo)
{
  unsigned int sample_rate = 44100;
  // No automatic sample rate conversion, change the sample rate to one supported:
  assert(deviceinfo.sampleRates.size() > 0);
  auto r = std::find(deviceinfo.sampleRates.cbegin(), deviceinfo.sampleRates.cend(), sample_rate);

  auto supported = r != deviceinfo.sampleRates.cend();
  
  if(!supported)
    sample_rate = deviceinfo.sampleRates.back();

  return sample_rate;
}



int main(int argc, char * argv[])
{
  if(argc != 2)
    {
      std::cerr << argv[0] << ": Incorrect number of arguments.\n";
      std::cerr << "Usage: " << argv[0] << " <file.sbp>\n";
      return EXIT_FAILURE;
    }
  
  auto text = LoadText(argv[1]);
  if(text.empty())
    return EXIT_FAILURE;

  auto json = LoadJson(text);
  if(!json)
    return EXIT_FAILURE;

  Blueprint blueprint;
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
  
  auto sample_rate = GetSampleRate(deviceinfo);
  //blueprint->SetSampleRate(sample_rate); // todo
  assert(sample_rate == 44100);

  NodeAudioDeviceOutput * n = nullptr;
  auto ados = blueprint.GetNodesByType("AudioDeviceOutput");
  if(ados.size() > 0)
    n = dynamic_cast<NodeAudioDeviceOutput *>(ados[0]);
  else
    std::cout << argv[0] << ": Warning: No AudioDeviceOutput node present, nothing will be output.\n";

  auto callbackdata = std::make_pair(&blueprint, n);


  // if(verbose) std::cout << argv[0] << ": Using device " << deviceinfo.name << ", sample rate " << sample_rate << std::endl;
  
  int rv = EXIT_SUCCESS;
  try
    {
      unsigned int bframes = 1024;
      dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT64, sample_rate, &bframes, playback, &callbackdata);
      dac->startStream();
    }
  catch(RtAudioError & e)
    {
      e.printMessage();
      rv = EXIT_FAILURE;
    }

  if(rv == EXIT_SUCCESS)
    done.wait();

  
  delete dac;
  
  return rv;
}
