#ifndef NODE_HH_
#define NODE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

// Do not edit the following (it is modified by the install script):
#ifndef LIBFMSYNTH_ENABLE_NODETESTING
# define LIBFMSYNTH_ENABLE_NODETESTING 1
#endif
// End of "do not edit".

#include "Input.hh"
#include "Output.hh"
#include <array>
#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <json11.hpp>


namespace fmsynth
{
  class Node
  {
  public:
    enum class Channel
      {
        Amplitude,
        Form,
        Aux
      };
    static constexpr std::array AllChannels { Channel::Amplitude, Channel::Form, Channel::Aux };

    [[nodiscard]] static std::shared_ptr<Node> Create(const json11::Json & json);
    

    [[nodiscard]] static std::string ChannelToString(Channel channel)
    {
      switch(channel)
        {
        case Channel::Amplitude: return "Amplitude";
        case Channel::Form:      return "Form";
        case Channel::Aux:       return "Aux";
        }
      assert(false);
      return "FATAL ERROR, ILLEGAL CHANNEL ENUM";
    }
    [[nodiscard]] static Channel StringToChannel(const std::string & string)
    {
      if(string == "Amplitude") return Channel::Amplitude;
      else if(string == "Form") return Channel::Form;
      else if(string == "Aux")  return Channel::Aux;
      assert(false);
      return Channel::Aux;
    }


    Node(const std::string & type);
    virtual ~Node();

    [[nodiscard]] const std::string & GetNodeType() const;
    [[nodiscard]] const std::string & GetId()   const;
    void                              SetId(const std::string & id);

    [[nodiscard]] virtual Input::Range GetInputRange(Channel channel) const;
    [[nodiscard]] virtual Input::Range GetFormOutputRange()     const;

    void                       SetSamplesPerSecond(unsigned int samples_per_second);
    [[nodiscard]] unsigned int GetSamplesPerSecond() const;

    void    PushInput(Node * pusher, Channel channel, double value);
    void    FinishFrame(long time_index);

#if LIBFMSYNTH_ENABLE_NODETESTING
    [[nodiscard]] double  GetLastFrame() const;
#endif
    
    [[nodiscard]] bool    IsEnabled() const;
    void                  SetEnabled(const Node * root, bool enabled);

    [[nodiscard]] bool    IsFinished() const;
    void                  SetIsFinished();

    [[nodiscard]] std::set<Node *> GetAllOutputNodes() const;
    virtual void                   ResetTime();

    [[nodiscard]] virtual json11::Json to_json() const;
    virtual void                       SetFromJson(const json11::Json & json);
  
    [[nodiscard]] const Input * GetInput(Channel channel) const;
    [[nodiscard]] Input *       GetInput(Channel channel);
    [[nodiscard]] Output *      GetOutput(Channel channel);

    void AddInputNode(Channel from_channel, Node * from_node);
    void AddOutputNode(Channel to_channel, Node * to_node);
    void RemoveInputNode(Channel channel, Node * node);
    void RemoveOutputNode(Channel channel, Node * node);
    
  protected:
    virtual void   OnInputConnected(Node * from);
    virtual double ProcessInput(double time, double form) = 0;
    virtual void   OnEnabled();
    virtual void   OnEOF();

    void          SetPreprocessAmplitude();
    void          SetOutputRange(Input::Range range);

  private:
    static unsigned long _next_id;
  
    std::string  _type;
    std::string  _id;
    bool         _preprocess_amplitude;

    bool         _enabled;
    unsigned int _samples_per_second;
    bool         _finished;
    std::array<Input,  AllChannels.size()> _inputs;
    std::array<Output, AllChannels.size()> _outputs;
    
    Input::Range _output_range;
#if LIBFMSYNTH_ENABLE_NODETESTING
    double       _last_frame;
#endif

    void UpdateNextId();
  };
}

#endif
