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
#include <vector>


static void Test()
{
  {
    fmsynth::Blueprint bp;
    testAssert("Blueprint has root node.", bp.GetRoot());
  }
  {
    fmsynth::Blueprint bp;
    testAssert("Load empty json.", bp.Load("{}"));
  }
  {
    fmsynth::Blueprint bp;
    testAssert("Load emptyish json.", bp.Load("{\"links\": [], \"nodes\": []}"));
  }

  {
    fmsynth::Blueprint bp;
    fmsynth::NodeConstant node;
    bp.AddNode(&node);
    testAssert("GetNodesByType() returns added node.", bp.GetNodesByType(node.GetType()).size() > 0);
  }

  {
    fmsynth::Blueprint bp;
    fmsynth::NodeConstant node;
    node.SetId("owerowir");
    bp.AddNode(&node);
    testAssert("GetNode() returns added node.", bp.GetNode(node.GetId()));
  }

  {
    struct Example
    {
      bool        ends;
      std::string filename;
    };
    std::vector<Example> examples
      {
        { true,  "Echo.sbp"         },
        { true,  "FallingBomb.sbp"  },
        { false, "HelloWorld.sbp"   },
        { true,  "HitExplosion.sbp" },
        { true,  "Quack.sbp"        },
        { false, "Tremolo.sbp"      },
        { false, "Vibrato.sbp"      },
        { true,  "Weapon1.sbp"      },
        { false, "Weird1.sbp"       },
        { false, "Weird2.sbp"       },
      };
    for(auto e : examples)
      {
        std::string testname { "Load example '" + e.filename + "' succeeds." };

        auto [json, error] = fmsynth::util::LoadJsonFile(srcdir + "/../examples/" + e.filename);
        if(!json)
          {
            testSkip(testname, error);
            continue;
          }
        {
          fmsynth::Blueprint bp;
          testAssert(testname, bp.Load(*json));

          testAssert("After loading example '" + e.filename + "', the blueprint is not immediately finished.", !bp.IsFinished());
          
          if(e.ends)
            {
              bool endedok = false;
              unsigned int samplecount = 0;

              for(unsigned int i = 0; !endedok && i < 1000 * bp.GetSamplesPerSecond(); i++)
                {
                  bp.Tick(1);
                  if(bp.IsFinished())
                    endedok = true;
                  else
                    samplecount++;
                }
              testAssert("Playbacking example '" + e.filename + "' ends after some time.", endedok);

              bp.ResetTime();
              unsigned int reset_samplecount = 0;
              for(unsigned int i = 0; !bp.IsFinished() && i < 1000 * bp.GetSamplesPerSecond(); i++)
                {
                  bp.Tick(1);
                  if(!bp.IsFinished())
                    reset_samplecount++;
                }
              testComment << "samplecount=" << samplecount << ", reset_samplecount=" << reset_samplecount << "\n";
              testAssert("Playbacking example '" + e.filename + "' after ResetTime() produces the same number of samples as from the first playback.", samplecount == reset_samplecount);
            }
        }
        {
          testname = "Playbacking example '" + e.filename + "' produces samples.";
          fmsynth::Blueprint bp;
          bool loaded = bp.Load(*json);
          if(loaded)
            {
              bp.Tick(1);
              testAssert(testname, !bp.IsFinished());
            }
          else
            testSkip(testname, "Failed to load '" + e.filename + "'.");
        }
        {
          testname = "Playbacking example '" + e.filename + "' produces variation in the output.";

          fmsynth::Blueprint bp;
          bool loaded = bp.Load(*json);
          if(loaded)
            {
              fmsynth::Node * outputnode = nullptr;
              {
                auto nodes = bp.GetNodesByType("AudioDeviceOutput");
                if(!nodes.empty())
                  outputnode = nodes[0];
              }
              testAssert("Example '" + e.filename + "' has an audio device output node.", outputnode);
              
              if(outputnode)
                {
                  unsigned int variationcount = 0;
                  double lastval = 0;
                  for(unsigned int i = 0; !bp.IsFinished() && i < 10 * bp.GetSamplesPerSecond(); i++)
                    {
                      bp.Tick(1);
                      if(!bp.IsFinished())
                        {
                          auto v = outputnode->GetLastFrame();
                          if(lastval != v)
                            variationcount++;
                          lastval = v;
                        }
                    }
                  testAssert("Playbacking example '" + e.filename + "' provides some variation in output within the first 10 seconds.", variationcount > 100);
                }
              else
                testSkip(testname, "No audio device output node present in the example.");
            }
          else
            testSkip(testname, "Failed to load '" + e.filename + "'.");
        }
      }
  }
}
