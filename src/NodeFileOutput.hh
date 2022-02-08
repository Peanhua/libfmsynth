#ifndef NODE_FILE_OUTPUT_HH_
#define NODE_FILE_OUTPUT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"

template <class T> class AudioFile;

namespace fmsynth
{
  class NodeFileOutput : public Node
  {
  public:
    NodeFileOutput();
    ~NodeFileOutput();

    const std::string & GetFilename() const;
    void                SetFilename(const std::string & filename);

    json11::Json to_json() const                        override;
    void         SetFromJson(const json11::Json & json) override;
  
  protected:
    double ProcessInput(double time, double form) override;
    void   OnEOF() override;

  private:
    AudioFile<double> * _file;
    std::string         _filename;
  };
}

#endif
