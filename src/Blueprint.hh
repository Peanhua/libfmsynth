#ifndef BLUEPRINT_HH_
#define BLUEPRINT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Node.hh"
#include <mutex>
#include <vector>

namespace fmsynth
{
  class NodeConstant;


  class Blueprint
  {
  public:
    Blueprint();
    Blueprint(const Blueprint & src)             = delete;
    Blueprint(Blueprint && src)                  = delete;
    ~Blueprint();

    Blueprint & operator=(const Blueprint & rhs) = delete;
    Blueprint & operator=(Blueprint && rhs)      = delete;
    
    void Clear();
    [[nodiscard]] bool Load(const json11::Json & json);
    
    void AddNode(std::shared_ptr<Node> node);
    void RemoveNode(Node * node);

    void ConnectNodes(Node::Channel from_channel, Node * from_node, Node::Channel to_channel, Node * to_node);
    void DisconnectNodes(Node::Channel from_channel, Node * from_node, Node::Channel to_channel, Node * to_node);
    
    void ResetTime();
    void Tick(long samples);
    void SetIsFinished();
    [[nodiscard]] bool IsFinished() const;
    
    void                       SetSamplesPerSecond(unsigned int samples_per_second);
    [[nodiscard]] unsigned int GetSamplesPerSecond() const;
    
    [[nodiscard]] std::mutex & GetLockMutex();
    [[nodiscard]] Node *       GetRoot() const;
    [[nodiscard]] Node *       GetNode(const std::string & id) const;
    [[nodiscard]] std::vector<Node *> GetAllNodes() const;
    [[nodiscard]] std::vector<Node *> GetNodesByType(const std::string & type) const;
    
  protected:
    void SortNodesToExecutionOrder();

  private:
    NodeConstant *      _root;
    std::vector<std::shared_ptr<Node>> _shared_nodes; // Both _nodes and _shared_nodes contain the same pointers.
    std::vector<Node *> _nodes;
    std::vector<Node *> _exec_nodes;
    bool                _nodes_sorted;
    std::mutex          _lock_mutex;
    long                _time_index;
    unsigned int        _samples_per_second;

    void ResetExecutionOrder();
  };
}

#endif
