/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeComment.hh"
#include "WidgetBlueprint.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeComment.hh"
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDir>
#include "QtIncludeEnd.hh"


WidgetNodeComment::WidgetNodeComment(QWidget * parent, QWidget * stack_under)
  : WidgetNode(parent, nullptr, true, true),
    _ui_node_comment(new Ui::NodeComment),
    _hue(215.0 / 360.0),
    _size(200, 100)
{
  _selecting_selects_contents = true;
  SetNodeType("Comment");
  _ui_node->_enabled->setVisible(false);
  _ui_node->_input_container->setVisible(false);
  _ui_node->_output_container->setVisible(false);
  _ui_node->_input_form->SetIsOptional();

  _ui_node_comment->setupUi(_ui_node->_content);

  _ui_node_comment->_resizer->installEventFilter(this);
  _ui_node_comment->_comment->installEventFilter(this);

  _stack_under = stack_under;
  stackUnder(_stack_under);
  SetColor();
}


void WidgetNodeComment::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  SetSize();
  SetColor();
}


bool WidgetNodeComment::eventFilter(QObject * object, QEvent * event)
{
  auto rv = WidgetNode::eventFilter(object, event);
  if(!rv && object == _ui_node_comment->_resizer)
    {
      auto me = static_cast<QMouseEvent *>(event);
      if(event->type() == QEvent::MouseButtonPress && (me->buttons() & Qt::LeftButton))
        {
          _resize_start_pos = me->globalPos();
          _resize_start_size = _size;
          rv = true;
        }
      if(event->type() == QEvent::MouseButtonRelease && me->button() == Qt::LeftButton)
        {
          GetWidgetBlueprint()->PostEdit(this);
          rv = true;
        }
      else if(event->type() == QEvent::MouseMove && (me->buttons() & Qt::LeftButton))
        {
          auto change = me->globalPos() - _resize_start_pos;
          _size.setWidth(std::max(100, _resize_start_size.width()  + change.x()));
          _size.setHeight(std::max(50, _resize_start_size.height() + change.y()));
          SetSize();
          rv = true;
        }
    }
  if(!rv && object == _ui_node_comment->_comment)
    {
      if(event->type() == QEvent::MouseButtonPress)
        {
          auto me = static_cast<QMouseEvent *>(event);
          if(me->buttons() & Qt::LeftButton)
            EditComment();
          else
            EditColor();
          rv = true;
        }
    }
  return rv;
}

void WidgetNodeComment::SetSize()
{
  setMinimumSize(_size);
  setMaximumSize(_size);
  resize(_size);
  updateGeometry();
}


json11::Json WidgetNodeComment::to_json() const
{
  auto rv = WidgetNode::to_json().object_items();
  rv["comment_text"]      = _ui_node_comment->_comment->text().toStdString();
  rv["comment_color_hue"] = _hue;
  rv["comment_width"]     = _size.width();
  rv["comment_height"]    = _size.height();
  return rv;
}


void WidgetNodeComment::SetFromJson(const json11::Json & json)
{
  _ui_node_comment->_comment->setText(QString::fromStdString(json["comment_text"].string_value()));
  _size.setWidth(json["comment_width"].int_value());
  _size.setHeight(json["comment_height"].int_value());
  _hue = json["comment_color_hue"].number_value();
  WidgetNode::SetFromJson(json);
}


void WidgetNodeComment::EditComment()
{
  bool ok;
  auto comment = QInputDialog::getMultiLineText(this,
                                                "Edit comment",
                                                "Comment",
                                                _ui_node_comment->_comment->text(),
                                                &ok);
  if(ok)
    {
      _ui_node_comment->_comment->setText(comment);
      GetWidgetBlueprint()->PostEdit(this);
    }
}

void WidgetNodeComment::EditColor()
{
  auto color = QColorDialog::getColor(Qt::blue, this, "Choose comment color");
  if(color.isValid())
    {
      _hue = color.hueF();
      SetColor();
      GetWidgetBlueprint()->PostEdit(this);
    }
}



void WidgetNodeComment::resizeEvent(QResizeEvent * event)
{
  WidgetNode::resizeEvent(event);
  SetSize();
  GetWidgetBlueprint()->PostEdit(this);
}



void WidgetNodeComment::SetColor()
{
  double alpha = 1.0;

  auto c1 = QColor::fromHsvF(_hue, 0.36, 0.95, alpha);
  auto c2 = QColor::fromHsvF(_hue, 0.58, 0.92, alpha);
  auto c3 = QColor::fromHsvF(_hue, 0.85, 0.49, alpha);
  auto c4 = QColor::fromHsvF(_hue, 0.85, 0.20, alpha);
  auto c5 = QColor::fromHsvF(_hue, 0.86, 0.71, alpha);
  
  QPalette palette;
  QBrush brush(c1); // QColor(153, 193, 241, 255)
  brush.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
  QBrush brush1(c2); // QColor(98, 160, 234, 255)
  brush1.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Light, brush1);
  QBrush brush2(c3); // QColor(18, 65, 123, 255)
  brush2.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Dark, brush2);
  QBrush brush3(c4); // QColor(0, 0, 0, 255)
  brush3.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Text, brush3);
  palette.setBrush(QPalette::Active, QPalette::ButtonText, brush3);
  palette.setBrush(QPalette::Active, QPalette::Base, brush);
  QBrush brush4(c5); // QColor(26, 95, 180, 255)
  brush4.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Window, brush4);
  QBrush brush5(QColor(0, 0, 0, 128));
  brush5.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush5);
#endif
  palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
  palette.setBrush(QPalette::Inactive, QPalette::Light, brush1);
  palette.setBrush(QPalette::Inactive, QPalette::Dark, brush2);
  palette.setBrush(QPalette::Inactive, QPalette::Text, brush3);
  palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush3);
  palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
  palette.setBrush(QPalette::Inactive, QPalette::Window, brush4);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush5);
#endif
  palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush2);
  palette.setBrush(QPalette::Disabled, QPalette::Light, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::Dark, brush2);
  palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
  palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
  palette.setBrush(QPalette::Disabled, QPalette::Base, brush4);
  palette.setBrush(QPalette::Disabled, QPalette::Window, brush4);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush5);
#endif
  setPalette(palette);
}
