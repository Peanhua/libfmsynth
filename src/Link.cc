/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Link.hh"
#include "Node.hh"
#include "WidgetBlueprint.hh"
#include "WidgetConnector.hh"
#include "WidgetLine.hh"
#include "WidgetNode.hh"
#include <cassert>


Link::Link(WidgetBlueprint * blueprint, WidgetNode * from_node, WidgetNode * to_node, const std::string & to_channel)
  : _blueprint(blueprint),
    _from_node(from_node),
    _to_node(to_node),
    _to_channel(to_channel),
    _line(nullptr)
{
  assert(blueprint);
  assert(from_node);
  assert(to_node);
  Update(from_node);
}


Link::~Link()
{
  auto nfrom = _from_node->GetNode();
  auto nto   = _to_node->GetNode();
  if(_to_channel == "Amplitude")
    nto->RemoveAmplitudeInputNode(nfrom);
  else if(_to_channel == "Form")
    nto->RemoveFormInputNode(nfrom);
  else if(_to_channel == "Aux")
    nto->RemoveAuxInputNode(nfrom);
  else
    assert(false);
  
  delete _line;
}


void Link::Update(const WidgetNode * node)
{
  if(node != _from_node && node != _to_node)
    return;
  
  delete _line;

  _line = new WidgetLine(_blueprint, GetFromPosition(), GetToPosition());
  _line->show();
  _line->setDisabled(true);
  _line->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

static QPoint GetGlobalPos(QWidget * for_widget)
{
  return for_widget->mapToGlobal(for_widget->rect().topLeft());
}

QPoint Link::GetFromPosition() const
{
  auto connector = _from_node->GetWidgetConnector("");
  auto halfsize = connector->size() / 2;
  QPoint center(halfsize.width(), halfsize.height());

  return GetGlobalPos(connector) - GetGlobalPos(_blueprint) + center;
}

QPoint Link::GetToPosition() const
{
  auto connector = _to_node->GetWidgetConnector(_to_channel);
  auto halfsize = connector->size() / 2;
  QPoint center(halfsize.width(), halfsize.height());

  return GetGlobalPos(connector) - GetGlobalPos(_blueprint) + center;
}



bool Link::Match(const WidgetNode * node, const std::string & channel) const
{
  if(node == _from_node && channel == "")
    return true;
  if(node == _to_node && channel == _to_channel)
    return true;
  return false;
}


WidgetNode * Link::GetFromNode()  const
{
  return _from_node;
}

WidgetNode * Link::GetToNode() const
{
  return _to_node;
}

const std::string & Link::GetToChannel() const
{
  return _to_channel;
}

json11::Json Link::to_json() const
{
  return json11::Json::object {
    { "from",       _from_node->GetNodeId() },
    { "to",         _to_node->GetNodeId()   },
    { "to_channel", _to_channel             }
  };
}
