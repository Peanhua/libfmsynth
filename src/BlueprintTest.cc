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
#include "NodeConstant.hh"
#include "Test.hh"
#include "Util.hh"


static int Test()
{
  int id = 1;

  {
    fmsynth::Blueprint bp;
    testAssert(id++, "Blueprint has root node.", bp.GetRoot());
  }
  {
    fmsynth::Blueprint bp;
    testAssert(id++, "Load empty json.", bp.Load("{}"));
  }
  {
    fmsynth::Blueprint bp;
    testAssert(id++, "Load emptyish json.", bp.Load("{\"links\": [], \"nodes\": []}"));
  }

  {
    fmsynth::Blueprint bp;
    fmsynth::NodeConstant node;
    bp.AddNode(&node);
    testAssert(id++, "GetNodesByType() returns added node.", bp.GetNodesByType(node.GetType()).size() > 0);
  }

  {
    fmsynth::Blueprint bp;
    fmsynth::NodeConstant node;
    node.SetId("owerowir");
    bp.AddNode(&node);
    testAssert(id++, "GetNode() returns added node.", bp.GetNode(node.GetId()));
  }

  {
    std::array examples
      {
        "Echo.sbp",
        "HelloWorld.sbp",
        "HitExplosion.sbp",
        "Tremolo.sbp",
        "Vibrato.sbp",
        "Weapon1.sbp",
        "Weird1.sbp",
        "Weird2.sbp",
      };
    for(auto e : examples)
      {
        std::string testname { std::string("Load example '") + e + "' succeeds." };

        auto [json, error] = fmsynth::util::LoadJsonFile(srcdir + "/../examples/" + e);
        if(!json)
          {
            testSkip(id++, testname, error);
            continue;
          }
        fmsynth::Blueprint bp;
        testAssert(id++, testname, bp.Load(*json));
      }
  }

  return id;
}
