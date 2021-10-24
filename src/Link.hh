#ifndef WIDGET_LINK_HH_
#define WIDGET_LINK_HH_
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
#include "QtIncludeBegin.hh"
#include <QtCore/QPoint>
#include "QtIncludeEnd.hh"

class Node;
class WidgetBlueprint;
class WidgetNode;
class WidgetLine;


class Link
{
public:
  Link(WidgetBlueprint * blueprint, WidgetNode * from_node, WidgetNode * to_node, const std::string & to_channel);
  ~Link();

  void Update(WidgetNode * node);
  bool Match(const WidgetNode * node, const std::string & channel) const;

  WidgetNode *        GetFromNode()     const;
  WidgetNode *        GetToNode()       const;
  const std::string & GetToChannel()    const;
  
  json11::Json to_json() const;
  
private:
  WidgetBlueprint * _blueprint;
  WidgetNode *      _from_node;
  WidgetNode *      _to_node;
  std::string       _to_channel;
  WidgetLine *      _line;

  QPoint              GetFromPosition() const;
  QPoint              GetToPosition()   const;
};

#endif
