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
#include "NodeInverse.hh"
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
    auto node1 = std::make_shared<fmsynth::NodeConstant>();
    {
      bp.AddNode(node1);
      testAssert("GetNodesByType() returns the added NodeConstant.", bp.GetNodesByType(node1->GetType()).size() > 0);
    }
    auto node2 = std::make_shared<fmsynth::NodeInverse>();
    {
      bp.AddNode(node2);
      testAssert("GetNodesByType() returns the added NodeInverse.", bp.GetNodesByType(node2->GetType()).size() > 0);
    }
  }

  {
    fmsynth::Blueprint bp;
    auto node = std::make_shared<fmsynth::NodeConstant>();
    node->SetId("owerowir");
    bp.AddNode(node);
    testAssert("GetNode() returns added node.", bp.GetNode(node->GetId()));
  }

  {
    struct OrderingInstruction
    {
      std::vector<std::string> node_ids;
    };
    struct Example
    {
      bool        ends;
      std::string filename;
      std::vector<OrderingInstruction> node_ordering_instructions;
    };
    std::vector<Example> examples
      {
        { true,  "Echo.sbp",         { }
        },
        { true,  "FallingBomb.sbp",  { }
        },
        { false, "HeartBeat.sbp",    { }
        },
        { false, "HelloWorld.sbp",   { }
        },
        { true,  "HitExplosion.sbp", { }
        },
        { true,  "Quack.sbp",        { }
        },
        { false, "Tremolo.sbp",      { }
        },
        { false, "Vibrato.sbp",      { }
        },
        { true,  "Weapon1.sbp",      { { { "amplitude_sine_hz",      "form_sawtooth_hz" } },
                                       { { "amplitude_oscillator",   "form_oscillator"  } },
                                       { { "amplitude_rangeconvert", "form_envelope"    } },
                                       { { "amplitude_math1"                            } },
                                       { { "amplitude_math2"                            } },
                                       { { "output"                                     } } }
        },
        { false, "Weird1.sbp",       { }
        },
        { false, "Weird2.sbp",       { }
        },
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
                  testname = "Playbacking example '" + e.filename + "' provides some variation in output within the first 10 seconds.";
#if LIBFMSYNTH_ENABLE_NODETESTING
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
                  testAssert(testname, variationcount > 100);
#else
                  testSkip(testname, "NodeTesting is disabled.");
#endif
                }
              else
                testSkip(testname, "No audio device output node present in the example.");
            }
          else
            testSkip(testname, "Failed to load '" + e.filename + "'.");
        }
        {
          testname = "Sorting the nodes of example '" + e.filename + "' produces the correct order.";

          auto & instructions = e.node_ordering_instructions;
          if(!instructions.empty())
            {
              class MyBlueprint : public fmsynth::Blueprint
              {
              public:
                void PublicSortNodesToExecutionOrder() { SortNodesToExecutionOrder(); }
              };
              
              MyBlueprint bp;
              bool loaded = bp.Load(*json);
              if(loaded)
                {
                  bp.PublicSortNodesToExecutionOrder();
                  auto nodes = bp.GetAllNodes();
                  bool success = true;
                  unsigned int current_index = 0;
                  for(auto instr : instructions)
                    {
                      for(auto id : instr.node_ids)
                        {
                          bool found = false;
                          for(unsigned int i = 0; !found && i < instr.node_ids.size(); i++)
                            {
                              auto curind = current_index + i;
                              if(nodes.size() > curind)
                                if(id == nodes[curind]->GetId())
                                  found = true;
                            }
                          if(!found)
                            {
                              testComment << "expected node(id=" << id << ") not found within nodes indices " << current_index << ".." << (static_cast<int>(current_index + instr.node_ids.size()) - 1) << "\n";
                              success = false;
                            }
                        }
                      current_index += static_cast<unsigned int>(instr.node_ids.size());
                    }
                  
                  testAssert(testname, success);
                }
              else
                testSkip(testname, "Failed to load '" + e.filename + "'.");
            }
          else
            testSkip(testname, "No test data defined.");
        }
        delete json;
      }
  }
}
