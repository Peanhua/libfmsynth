#ifndef WIDGET_BLUEPRINT_HH_
#define WIDGET_BLUEPRINT_HH_
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
#include <set>
#include <vector>
#include "QtIncludeBegin.hh"
#include <QtWidgets/QWidget>
#include "QtIncludeEnd.hh"

class Blueprint;
class Link;
class Node;
class QScrollArea;
class WidgetMainWindow;
class WidgetNode;


class WidgetBlueprint : public QWidget
{
public:
  WidgetBlueprint(QWidget * parent = nullptr);
  ~WidgetBlueprint();
  
  void dragEnterEvent(QDragEnterEvent *event)        override;
  void dropEvent(QDropEvent *event)                  override;
  void dragMoveEvent(QDragMoveEvent *event)          override;
  void dragLeaveEvent(QDragLeaveEvent *event)        override;
  bool eventFilter(QObject * object, QEvent * event) override;

  WidgetMainWindow * GetMainWindow() const;
  QScrollArea *      GetScrollArea() const;
  
  void SetFilename(const std::string & filename);
  void Save();
  void Save(const std::string & filename);
  void Load();
  void Load(const std::string & filename);
  void Load(const json11::Json & json);
  void Reset();
  const std::string & GetFilename() const;
  
  Blueprint *  GetBlueprint() const;
  WidgetNode * AddNode(int x, int y, const std::string & node_type);
  void         AddLink(WidgetNode * from_node, WidgetNode * to_node, const std::string & to_channel);
  void         DeleteNode(WidgetNode * node);
  void         DeleteLink(WidgetNode * node, const std::string & channel);
  void         DeleteLink(WidgetNode * node, const std::string & channel, WidgetNode * other);
  void         OnNodeMoved(WidgetNode * node);
  unsigned int CountLinks(WidgetNode * node, const std::string & channel) const;
  std::vector<Link *> GetLinks(const WidgetNode * node, const std::string & channel) const;
  bool         CanRun() const;
  Blueprint *  Build() const;
  bool         IsDirty() const;
  void         SetDirty(bool dirty);
  void         UpdateWindowTitle();

  void PostEdit();
  void PostEdit(QWidget * edited_node);
  void PostEdit(const std::set<QWidget *> & edited_nodes);
  
  void Undo();
  void Redo();
  bool CanUndo() const;
  bool CanRedo() const;

  unsigned int GetSelectedNodesCount() const;

  void NodeSelectionAll();
  void NodeSelectionAdd(WidgetNode * node);
  void NodeSelectionSet(WidgetNode * node);
  void NodeSelectionToggle(WidgetNode * node);

  void DeleteSelectedNodes();
  void MoveSelectedNodes(const QPoint & delta);

  void SetSnapToGrid(bool snap_to_grid);

  std::vector<WidgetNode *> GetOverlappingNodes(const WidgetNode * for_node) const;
  
  json11::Json to_json();
  
private:
  std::string               _filename;
  Blueprint *               _blueprint;
  std::vector<WidgetNode *> _nodes;
  std::vector<Link *>       _links;
  bool                      _dirty;
  bool                      _loading;
  QPoint                    _drag_last_pos;
  std::vector<json11::Json> _undobuffer;
  json11::Json              _post_edit_save;
  unsigned int              _undopos;
  bool                      _snap_to_grid;

  void UpdateNodesData();
  
  std::set<WidgetNode *> _selected_nodes;
  void NodeSelectionUpdated();
};

#endif
