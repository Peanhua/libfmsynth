#ifndef WIDGET_BLUEPRINT_HH_
#define WIDGET_BLUEPRINT_HH_
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
#include <set>
#include <vector>
#include <json11.hpp>
#include "QtIncludeBegin.hh"
#include <QtWidgets/QWidget>
#include "QtIncludeEnd.hh"

namespace fmsynth
{
  class Blueprint;
}
class Link;
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

  [[nodiscard]] WidgetMainWindow * GetMainWindow() const;
  [[nodiscard]] QScrollArea *      GetScrollArea() const;
  
  void SetFilename(const std::string & filename);
  void Save();
  void Save(const std::string & filename);
  void Load();
  void Load(const std::string & filename);
  void Load(const json11::Json & json);
  void Reset();
  [[nodiscard]] const std::string & GetFilename() const;
  
  [[nodiscard]] std::shared_ptr<fmsynth::Blueprint> GetBlueprint() const;
  [[nodiscard]] WidgetNode *                        AddNode(int x, int y, const std::string & node_type);
  void                               AddLink(WidgetNode * from_node, WidgetNode * to_node, fmsynth::Node::Channel to_channel);
  void                               DeleteNode(WidgetNode * node);
  void                               DeleteInputLink(WidgetNode * node, fmsynth::Node::Channel channel);
  void                               DeleteInputLink(WidgetNode * node, fmsynth::Node::Channel channel, WidgetNode * other);
  void                               DeleteOutputLink(WidgetNode * node, fmsynth::Node::Channel channel);
  [[nodiscard]] unsigned int         CountInputLinks(WidgetNode * to_node, fmsynth::Node::Channel to_channel)          const;
  [[nodiscard]] unsigned int         CountOutputLinks(WidgetNode * from_node, fmsynth::Node::Channel from_channel)     const;
  [[nodiscard]] std::vector<Link *>  GetInputLinks(const WidgetNode * to_node, fmsynth::Node::Channel to_channel)      const;
  [[nodiscard]] std::vector<Link *>  GetOutputLinks(const WidgetNode * from_node, fmsynth::Node::Channel from_channel) const;
  void                               UpdateLinks(const WidgetNode * node);
  [[nodiscard]] bool                 CanRun() const;
  [[nodiscard]] std::shared_ptr<fmsynth::Blueprint> Build() const;
  [[nodiscard]] bool                 IsDirty() const;
  void                               SetDirty(bool dirty);
  void                               UpdateWindowTitle();

  void PostEdit();
  void PostEdit(QWidget * edited_node);
  void PostEdit(const std::set<QWidget *> & edited_nodes);
  
  void               Undo();
  void               Redo();
  [[nodiscard]] bool CanUndo() const;
  [[nodiscard]] bool CanRedo() const;

  [[nodiscard]] unsigned int GetSelectedNodesCount() const;

  void NodeSelectionAll();
  void NodeSelectionAdd(WidgetNode * node);
  void NodeSelectionSet(WidgetNode * node);
  void NodeSelectionToggle(WidgetNode * node);

  void DeleteSelectedNodes();
  void MoveSelectedNodes(const QPoint & delta);

  void SetSnapToGrid(bool snap_to_grid);

  [[nodiscard]] std::vector<WidgetNode *> GetOverlappingNodes(const WidgetNode * for_node) const;
  
  [[nodiscard]] json11::Json to_json();
  
private:
  std::string                         _filename;
  std::shared_ptr<fmsynth::Blueprint> _blueprint;
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

  void DeleteLink(std::function<bool(const Link *)> match_callback);
};

#endif
