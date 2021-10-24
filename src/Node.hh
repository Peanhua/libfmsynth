#ifndef NODE_HH_
#define NODE_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Input.hh"
#include "Types.hh"
#include <map>
#include <memory>
#include <set>
#include <string>


class Node
{
public:
  Node(const std::string & type);
  virtual ~Node();

  static Node * Create(const json11::Json & json);

  const std::string & GetType() const;
  const std::string & GetId()   const;
  void                SetId(const std::string & id);
  
  void    AddAmplitudeInputNode(Node * node);
  void    AddFormInputNode(Node * node);
  void    AddAuxInputNode(Node * node);
  void    RemoveAmplitudeInputNode(Node * node);
  void    RemoveFormInputNode(Node * node);
  void    RemoveAuxInputNode(Node * node);

  virtual Input::Range GetAmplitudeInputRange() const;
  Input::Range         GetFormInputRange()      const;
  virtual Input::Range GetAuxInputRange()       const;
  virtual Input::Range GetFormOutputRange()     const;

  void    PushAmplitudeInput(long time_index, Node * pusher, double amplitude);
  void    PushFormInput(long time_index, Node * pusher, double form);
  void    PushAuxInput(long time_index, Node * pusher, double value);

  bool    IsEnabled() const;
  void    SetEnabled(const Node * root, bool enabled);

  bool    IsFinished() const;
  void    SetIsFinished();

  std::set<Node *> GetAllConnectedNodes() const;
  virtual void     ResetTime();

  virtual json11::Json to_json() const;
  virtual void         SetFromJson(const json11::Json & json);
  
protected:
  virtual void   OnInputConnected(Node * from);
  virtual double ProcessInput(double time, double form) = 0;
  virtual void   OnEnabled();
  virtual void   OnEOF();

  void          SetPreprocessAmplitude();
  const Input * GetAmplitudeInput() const;
  const Input * GetFormInput()      const;
  Input *       GetFormInput();
  Input *       GetAuxInput();

private:
  static unsigned int _next_id;
  
  std::string _type;
  std::string _id;
  bool        _preprocess_amplitude;

  bool        _enabled;
  bool        _finished;
  Input       _amplitude;
  Input       _form;
  Input       _aux;

  void FinishFrame(long time_index);
};

#endif
