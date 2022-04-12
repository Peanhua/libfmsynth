#ifndef NODE_HH_
#define NODE_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
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

    static Node * Create(const json11::Json & json);

    static std::string ChannelToString(Channel channel)
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
    static Channel StringToChannel(const std::string & string)
    {
      if(string == "Amplitude") return Channel::Amplitude;
      else if(string == "Form") return Channel::Form;
      else if(string == "Aux")  return Channel::Aux;
      assert(false);
      return Channel::Aux;
    }


    Node(const std::string & type);
    virtual ~Node();

    const std::string & GetType() const;
    const std::string & GetId()   const;
    void                SetId(const std::string & id);

    void AddInputNode(Channel channel, Node * node);
    void RemoveInputNode(Channel channel, Node * node);

    virtual Input::Range GetInputRange(Channel channel) const;
    virtual Input::Range GetFormOutputRange()     const;

    void         SetSamplesPerSecond(unsigned int samples_per_second);
    unsigned int GetSamplesPerSecond() const;

    void    PushInput(Node * pusher, Channel channel, double value);
    void    FinishFrame(long time_index);

#if LIBFMSYNTH_ENABLE_NODETESTING
    double  GetLastFrame() const;
#endif
    
    bool    IsEnabled() const;
    void    SetEnabled(const Node * root, bool enabled);

    bool    IsFinished() const;
    void    SetIsFinished();

    std::set<Node *> GetAllConnectedNodes() const; // todo: Rename to match the behavior: This currently returns all output nodes.
    virtual void     ResetTime();

    virtual json11::Json to_json() const;
    virtual void         SetFromJson(const json11::Json & json);
  
    const Input * GetInput(Channel channel) const;
    Input *       GetInput(Channel channel);

  protected:
    virtual void   OnInputConnected(Node * from);
    virtual double ProcessInput(double time, double form) = 0;
    virtual void   OnEnabled();
    virtual void   OnEOF();

    void          SetPreprocessAmplitude();

  private:
    static unsigned int _next_id;
  
    std::string  _type;
    std::string  _id;
    bool         _preprocess_amplitude;

    bool         _enabled;
    unsigned int _samples_per_second;
    bool         _finished;
    Input        _amplitude;
    Input        _form;
    Input        _aux;
#if LIBFMSYNTH_ENABLE_NODETESTING
    double       _last_frame;
#endif

    void    AddAmplitudeInputNode(Node * node);
    void    AddFormInputNode(Node * node);
    void    AddAuxInputNode(Node * node);
    void    RemoveAmplitudeInputNode(Node * node);
    void    RemoveFormInputNode(Node * node);
    void    RemoveAuxInputNode(Node * node);

    void    PushAmplitudeInput(Node * pusher, double amplitude);
    void    PushFormInput(Node * pusher, double form);
    void    PushAuxInput(Node * pusher, double value);
  };
}

#endif
