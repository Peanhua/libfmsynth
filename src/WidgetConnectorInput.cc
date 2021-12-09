/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/
#include "WidgetConnectorInput.hh"
#include "Link.hh"
#include "WidgetBlueprint.hh"
#include "WidgetConnectorOutput.hh"
#include "WidgetNode.hh"
#include <cassert>
#include "QtIncludeBegin.hh"
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include "QtIncludeEnd.hh"


WidgetConnectorInput::WidgetConnectorInput(QWidget * parent)
  : WidgetConnector(parent)
{
  _is_input = true;
  SetIsConnected(false);
  setAcceptDrops(true);
  
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested,
          [this](const QPoint & pos)
          {
            auto blueprint = GetBlueprint();
            auto node    = GetNode(this);
            auto channel = property("InputChannel").toString().toStdString();
            if(blueprint->CountLinks(node, channel) <= 1)
              return;
            
            auto menu = new QMenu;
            for(auto link : blueprint->GetLinks(node, channel))
              {
                auto other = link->GetFromNode();
                if(other == node)
                  other = link->GetToNode();
                
                auto action = menu->addAction(QString::fromStdString("Delete link to " + other->GetNodeId() + " (" + other->GetNodeType() + ")"));
                action->setIcon(QIcon::fromTheme("edit-delete"));
                connect(action, &QAction::triggered,
                        [this, node, channel, other]([[maybe_unused]] bool checked)
                        {
                          GetBlueprint()->DeleteLink(node, channel, other);
                        });
              }
            menu->addSeparator();
            {
              auto action = menu->addAction(QString::fromStdString("Delete all links"));
              action->setIcon(QIcon::fromTheme("edit-delete"));
              connect(action, &QAction::triggered,
                      [this, node, channel]([[maybe_unused]] bool checked)
                      {
                        GetBlueprint()->DeleteLink(node, channel);
                      });
            }
            menu->exec(mapToGlobal(pos));
          });
}


void WidgetConnectorInput::dragEnterEvent(QDragEnterEvent *event)
{
  bool accept = true;

  auto src = dynamic_cast<WidgetConnectorOutput *>(event->source());
  if(!src)
    accept = false;

  if(src)
    {
      auto from_node = GetNode(src);
      auto to_node   = GetNode(this);
      if(from_node == to_node || from_node->DependsOn(to_node))
        accept = false;
    }

  if(accept)
    event->acceptProposedAction();
  else
    event->ignore();
}

void WidgetConnectorInput::dragLeaveEvent([[maybe_unused]] QDragLeaveEvent *event)
{
  event->ignore();
}


void WidgetConnectorInput::dropEvent(QDropEvent *event)
{
  auto src = dynamic_cast<WidgetConnectorOutput *>(event->source());
  if(!src)
    return;
  
  auto from_node = GetNode(src);
  auto to_node   = GetNode(this);
  if(from_node == to_node || from_node->DependsOn(to_node))
    return;
  
  auto blueprint = GetBlueprint();
  auto to_channel = property("InputChannel").toString().toStdString();
  auto output = property("NodeId").toString().toStdString() + "/" + to_channel;
  blueprint->AddLink(from_node, to_node, to_channel);
  blueprint->PostEdit({from_node, to_node});
}



void WidgetConnectorInput::mousePressEvent([[maybe_unused]] QMouseEvent * event)
{
  auto blueprint = GetBlueprint();
  auto node      = GetNode(this);
  auto channel   = property("InputChannel").toString().toStdString();
  if(blueprint->CountLinks(node, channel) == 1)
    blueprint->DeleteLink(node, channel);
}


WidgetBlueprint * WidgetConnectorInput::GetBlueprint() const
{
  auto blueprint = parentWidget();
  while(blueprint && blueprint->objectName().toStdString() != "_blueprint")
    blueprint = blueprint->parentWidget();
  assert(blueprint);
  return dynamic_cast<WidgetBlueprint *>(blueprint);
}


WidgetNode * WidgetConnectorInput::GetNode(QWidget * for_widget) const
{
  assert(for_widget);
  auto node = for_widget->parentWidget();
  while(node && node->objectName().toStdString() != "Node")
    node = node->parentWidget();
  assert(node);
  return dynamic_cast<WidgetNode *>(node);
}
