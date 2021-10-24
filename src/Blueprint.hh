#ifndef BLUEPRINT_HH_
#define BLUEPRINT_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Types.hh"
#include <mutex>
#include <string>
#include <vector>

class Node;
class NodeConstant;


class Blueprint
{
public:
  Blueprint();
  ~Blueprint();

  void Clear();
  bool Load(const json11::Json & json);

  void AddNode(Node * node);
  void RemoveNode(Node * node);
  void AddLink(Node * from, Node * to, const std::string & channel);

  void ResetTime();
  void Tick(long samples);
  void SetIsFinished();
  bool IsFinished() const;

  std::mutex & GetLockMutex();
  Node *       GetRoot() const;
  Node *       GetNode(const std::string & id) const;

private:
  NodeConstant *      _root;
  std::vector<Node *> _nodes;
  std::mutex          _lock_mutex;
  long                _time_index;
};

#endif
