#ifndef WIDGET_NODE_FILTER_HH_
#define WIDGET_NODE_FILTER_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"
#include "NodeFilter.hh"


class WidgetNodeFilter : public WidgetNode
{
public:
  WidgetNodeFilter(QWidget * parent);
  WidgetNodeFilter(QWidget * parent, fmsynth::NodeFilter::Type type);
  
  void   UpdateConnectorStates() override;
  
  void   NodeToWidget() override;
  void   WidgetToNode() override;

private:
  fmsynth::NodeFilter::Type _type;
  fmsynth::NodeFilter *     _node_filter;
  Ui::NodeFilter *          _ui_node_filter;

  std::string               FilterTypeString(fmsynth::NodeFilter::Type type) const;
  fmsynth::NodeFilter::Type FilterTypeFromString(const std::string & string) const;
  void                      UpdateFilterType();
};


#endif
