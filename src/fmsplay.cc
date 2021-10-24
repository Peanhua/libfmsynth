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
#include "Player.hh"
#include <cassert>
#include <fstream>
#include <iostream>
#include <alut.h>


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
  
  auto success = alutInit(nullptr, nullptr);
  if(!success)
    return EXIT_FAILURE;

  Player player;
  player.Start();
  player.SetNextProgram(&blueprint);

  bool done = false;
  while(!done)
    {
      {
        std::lock_guard lock(blueprint.GetLockMutex());
        if(blueprint.IsFinished())
          done = true;
      }
      if(!done)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  
  player.Stop();

  return EXIT_SUCCESS;
}
