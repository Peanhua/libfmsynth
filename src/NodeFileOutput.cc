/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "NodeFileOutput.hh"
#include <iostream>
#include <AudioFile.h>

using namespace fmsynth;


NodeFileOutput::NodeFileOutput()
  : Node("FileOutput"),
    _file(new AudioFile<double>()),
    _filename("")
{
  SetPreprocessAmplitude();
  _file->setNumChannels(1);
}


NodeFileOutput::~NodeFileOutput()
{
  delete _file;
}


const std::string & NodeFileOutput::GetFilename() const
{
  return _filename;
}


void NodeFileOutput::SetFilename(const std::string & filename)
{
  _filename = filename;
}


double NodeFileOutput::ProcessInput([[maybe_unused]] double time, double form)
{
  auto ind = _file->samples[0u].size();
  _file->setNumSamplesPerChannel(static_cast<int>(ind + 1));
  _file->samples[0u][ind] = form;

  return form;
}


void NodeFileOutput::OnEOF()
{
  std::cout << "Writing " << _filename << std::endl;
  _file->save(_filename);
}    


json11::Json NodeFileOutput::to_json() const
{
  auto rv = Node::to_json().object_items();
  rv["fileoutput_filename"] = _filename;
  return rv;
}

void NodeFileOutput::SetFromJson(const json11::Json & json)
{
  Node::SetFromJson(json);
  _filename = json["constant_value"].string_value();
}
