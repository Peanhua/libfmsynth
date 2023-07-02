#ifndef WIDGET_CONNECTOR_HH_
#define WIDGET_CONNECTOR_HH_
/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "QtIncludeBegin.hh"
#include <QtWidgets/QPushButton>
#include "QtIncludeEnd.hh"

class WidgetBlueprint;
class WidgetNode;


class WidgetConnector : public QPushButton
{
public:
  WidgetConnector(QWidget * parent);

  const WidgetNode * GetOwner() const;

  void SetOwner(WidgetBlueprint * blueprint, WidgetNode * node);
  void SetRangeFilename(const std::string & range_filename);
  
  [[nodiscard]] bool IsOptional() const;
  void               SetIsOptional();

  [[nodiscard]] bool IsConnected() const;
  void               SetIsConnected(bool is_connected);

  void UpdateVisuals();
  
  void moveEvent(QMoveEvent *event) override;
  
protected:
  bool _is_input;

private:
  WidgetBlueprint * _blueprint;
  WidgetNode *      _node;
  bool              _is_optional;
  std::string       _range_filename;
  bool              _connected;
};

#endif
