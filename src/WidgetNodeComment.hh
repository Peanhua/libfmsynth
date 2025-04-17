#ifndef WIDGET_NODE_COMMENT_HH_
#define WIDGET_NODE_COMMENT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/


#include "WidgetNode.hh"


class WidgetNodeComment : public WidgetNode
{
public:
  WidgetNodeComment(QWidget * parent, QWidget * stack_under);

  [[nodiscard]] bool         eventFilter(QObject * object, QEvent * event) override;
  void                       resizeEvent(QResizeEvent * event)             override;
  
  void                       NodeToWidget()                         override;

  [[nodiscard]] json11::Json to_json() const                        override;
  void                       SetFromJson(const json11::Json & json) override;
  
private:
  Ui::NodeComment * _ui_node_comment;
  double            _hue;
  QSize             _size;
  QPoint            _resize_start_pos;
  QSize             _resize_start_size;

  void SetSize();
  void SetColor();
  void EditComment();
  void EditColor();
};


#endif
