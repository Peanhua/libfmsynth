/*
  libfmsynth
  Copyright (C) 2021-2023  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetBlueprint.hh"
#include "Blueprint.hh"
#include "Link.hh"
#include "NodeConstant.hh"
#include "Player.hh"
#include "Util.hh"
#include "WidgetAddNode.hh"
#include "WidgetMainWindow.hh"
#include "WidgetNodeADHSR.hh"
#include "WidgetNodeAdd.hh"
#include "WidgetNodeAudioDeviceOutput.hh"
#include "WidgetNodeAverage.hh"
#include "WidgetNodeClamp.hh"
#include "WidgetNodeComment.hh"
#include "WidgetNodeConstant.hh"
#include "WidgetNodeDelay.hh"
#include "WidgetNodeFileOutput.hh"
#include "WidgetNodeFilter.hh"
#include "WidgetNodeGrowth.hh"
#include "WidgetNodeInverse.hh"
#include "WidgetNodeMultiply.hh"
#include "WidgetNodeOscillator.hh"
#include "WidgetNodeRangeConvert.hh"
#include "WidgetNodeReciprocal.hh"
#include "WidgetNodeViewWaveform.hh"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "QtIncludeBegin.hh"
#include <QFileDialog>
#include <QtGui/QDrag>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStatusBar>
#include "QtIncludeEnd.hh"


WidgetBlueprint::WidgetBlueprint(QWidget * parent)
  : QWidget(parent),
    _blueprint(new fmsynth::Blueprint),
    _dirty(false),
    _loading(false),
    _undopos(0),
    _snap_to_grid(false)
{
  installEventFilter(this);
  _post_edit_save = to_json();
}


WidgetBlueprint::~WidgetBlueprint()
{
  for(auto l : _links)
    delete l;
  for(auto n : _nodes)
    if(n)
      n->deleteLater();
}

QScrollArea * WidgetBlueprint::GetScrollArea() const
{
  auto sa = parentWidget();
  while(sa && sa->objectName().toStdString() != "_scrollArea_Blueprint")
    sa = sa->parentWidget();

  return static_cast<QScrollArea *>(sa);
}

bool WidgetBlueprint::eventFilter(QObject * object, QEvent * event)
{
  bool rv = false;
  auto widget = dynamic_cast<QWidget *>(object);
  
  //  if(object == this)
    {
      auto me = static_cast<QMouseEvent *>(event);
      if(event->type() == QEvent::MouseButtonPress)
        {
          if(object == this && (me->buttons() & Qt::LeftButton))
            NodeSelectionSet(nullptr);
          if(me->buttons() & Qt::MiddleButton) // todo: Make the used mousebutton user configurable.
            {
              assert(widget);
              _drag_last_pos = widget->mapToGlobal(me->pos());
              rv = true; // todo: We should probably not "eat" the event, unless we're using the left mousebutton (used to drag other stuff).
            }
        }
      else if(event->type() == QEvent::MouseMove)
        {
          if(me->buttons() & Qt::MiddleButton)
            {
              assert(widget);
              auto epos = widget->mapToGlobal(me->pos());
              QPoint delta = epos - _drag_last_pos;
              delta *= 1; // todo: Make this user configurable.
              _drag_last_pos = epos;

              auto sarea = GetScrollArea();
              assert(sarea);
              auto h = sarea->horizontalScrollBar();
              auto v = sarea->verticalScrollBar();
              h->setValue(h->value() - delta.x());
              v->setValue(v->value() - delta.y());
              rv = true; // todo: We should probably not "eat" the event, the used button in mouse move event probably does not matter like it does for mouse button press event.
            }
        }
    }
  return rv;
}


void WidgetBlueprint::dragEnterEvent(QDragEnterEvent *event)
{
  if(dynamic_cast<WidgetAddNode *>(event->source()))
    event->acceptProposedAction();
}

void WidgetBlueprint::dragLeaveEvent([[maybe_unused]] QDragLeaveEvent *event)
{
  event->ignore();
}


void WidgetBlueprint::dragMoveEvent(QDragMoveEvent *event)
{
  auto child = childAt(event->pos());

  if(!child)
    {
      event->accept();
      return;
    }

  while(child)
    {
      if(dynamic_cast<WidgetNodeComment *>(child))
        {
          event->accept();
          return;
        }
      child = child->parentWidget();
    }
  
  event->ignore();
}

void WidgetBlueprint::dropEvent(QDropEvent *event)
{
  auto node = AddNode(event->pos().x(), event->pos().y(), event->source()->property("NodeType").toString().toStdString());
  PostEdit(node);
}


WidgetNode * WidgetBlueprint::AddNode(int x, int y, const std::string & node_type)
{
  WidgetNode * nodewidget = nullptr;
  if(node_type == "Constant")
    nodewidget = new WidgetNodeConstant(this);
  else if(node_type == "Growth")
    nodewidget = new WidgetNodeGrowth(this);
  else if(node_type == "FileOutput")
    nodewidget = new WidgetNodeFileOutput(this);
  else if(node_type == "AudioDeviceOutput")
    nodewidget = new WidgetNodeAudioDeviceOutput(this);
  else if(node_type == "Sine" || node_type == "Oscillator")
    nodewidget = new WidgetNodeOscillator(this, fmsynth::NodeOscillator::Type::SINE);
  else if(node_type == "Pulse")
    nodewidget = new WidgetNodeOscillator(this, fmsynth::NodeOscillator::Type::PULSE);
  else if(node_type == "Triangle")
    nodewidget = new WidgetNodeOscillator(this, fmsynth::NodeOscillator::Type::TRIANGLE);
  else if(node_type == "Sawtooth")
    nodewidget = new WidgetNodeOscillator(this, fmsynth::NodeOscillator::Type::SAWTOOTH);
  else if(node_type == "Noise")
    nodewidget = new WidgetNodeOscillator(this, fmsynth::NodeOscillator::Type::NOISE);
  else if(node_type == "ADHSR")
    nodewidget = new WidgetNodeADHSR(this);
  else if(node_type == "Add")
    nodewidget = new WidgetNodeAdd(this);
  else if(node_type == "Inverse")
    nodewidget = new WidgetNodeInverse(this);
  else if(node_type == "Multiply")
    nodewidget = new WidgetNodeMultiply(this);
  else if(node_type == "Average")
    nodewidget = new WidgetNodeAverage(this);
  else if(node_type == "Filter")
    nodewidget = new WidgetNodeFilter(this);
  else if(node_type == "LowPass")
    nodewidget = new WidgetNodeFilter(this, fmsynth::NodeFilter::Type::LOW_PASS);
  else if(node_type == "HighPass")
    nodewidget = new WidgetNodeFilter(this, fmsynth::NodeFilter::Type::HIGH_PASS);
  else if(node_type == "RangeConvert")
    nodewidget = new WidgetNodeRangeConvert(this);
  else if(node_type == "Clamp")
    nodewidget = new WidgetNodeClamp(this);
  else if(node_type == "Reciprocal")
    nodewidget = new WidgetNodeReciprocal(this);
  else if(node_type == "Delay")
    nodewidget = new WidgetNodeDelay(this);
  else if(node_type == "Comment")
    nodewidget = new WidgetNodeComment(this, GetMainWindow()->GetCommentBorderWidget());
  else if(node_type == "ViewWaveform")
    nodewidget = new WidgetNodeViewWaveform(this);
  else
    std::cout << "AddNode(): Invalid node type '" << node_type << "'" << std::endl;

  if(nodewidget)
    {
      nodewidget->setObjectName(QString::fromUtf8("Node"));
      nodewidget->UpdateNodeId();

      QPoint pos {x, y};
      if(_snap_to_grid)
        {
          auto snap = [](int v) { return v - v % 20; };
          pos.setX(snap(pos.x()));
          pos.setY(snap(pos.y()));
        }
      nodewidget->move(pos);
      
      nodewidget->show();
      {
        std::lock_guard lock(_blueprint->GetLockMutex());
        _nodes.push_back(nodewidget);
        _blueprint->AddNode(nodewidget->GetSharedNode());
      }
    }

  return nodewidget;
}


void WidgetBlueprint::AddLink(WidgetNode * from_node, WidgetNode * to_node, fmsynth::Node::Channel to_channel)
{
  if(!to_node->IsMultiInput(to_channel))
    DeleteInputLink(to_node, to_channel);

  {
    std::lock_guard lock(_blueprint->GetLockMutex());
    _links.push_back(new Link(this, from_node, to_node, to_channel));
  }
  
  // todo: Update only the connectors that actually change, not all connectors of both nodes.
  from_node->UpdateConnectorStates();
  to_node->UpdateConnectorStates();
}


void WidgetBlueprint::UpdateLinks(const WidgetNode * node)
{
  for(auto link : _links)
    if(link)
      link->Update(node);
}


void WidgetBlueprint::DeleteNode(WidgetNode * node)
{
  if(!node)
    return;

  DeleteOutputLink(node, fmsynth::Node::Channel::Form);
  DeleteInputLink(node, fmsynth::Node::Channel::Amplitude);
  DeleteInputLink(node, fmsynth::Node::Channel::Form);
  DeleteInputLink(node, fmsynth::Node::Channel::Aux);

  {
    std::lock_guard lock(_blueprint->GetLockMutex());

    _blueprint->RemoveNode(node->GetNode());
  
    for(unsigned int i = 0; i < _nodes.size(); i++)
      if(_nodes[i] == node)
        _nodes[i] = nullptr;

    if(node)
      node->deleteLater();
    
    PostEdit();
  }
}


void WidgetBlueprint::DeleteLink(std::function<bool(const Link *)> match_callback)
{
  std::set<QWidget *> edited_nodes;
  std::lock_guard lock(_blueprint->GetLockMutex());

  for(unsigned int i = 0; i < _links.size(); i++)
    {
      auto link = _links[i];
      if(link && match_callback(link))
        {
          auto from_node = link->GetFromNode();
          auto to_node = link->GetToNode();

          delete link;
          _links[i] = nullptr;

          from_node->UpdateConnectorStates();
          to_node->UpdateConnectorStates();
          
          edited_nodes.insert(from_node);
          edited_nodes.insert(to_node);
        }
    }

  PostEdit(edited_nodes);
}


void WidgetBlueprint::DeleteInputLink(WidgetNode * node, fmsynth::Node::Channel channel)
{
  DeleteLink([node, channel](const Link * link)
  {
    return
      node    == link->GetToNode() &&
      channel == link->GetToChannel();
  });
}


void WidgetBlueprint::DeleteInputLink(WidgetNode * node, fmsynth::Node::Channel channel, WidgetNode * other)
{
  DeleteLink([node, channel, other](const Link * link)
  {
    return
      node    == link->GetToNode() &&
      channel == link->GetToChannel() &&
      other   == link->GetFromNode();
  });
}


void WidgetBlueprint::DeleteOutputLink(WidgetNode * node, fmsynth::Node::Channel channel)
{
  assert(channel == fmsynth::Node::Channel::Form);

  DeleteLink([node, channel](const Link * link)
  {
    return
      node    == link->GetFromNode() &&
      channel == fmsynth::Node::Channel::Form; // todo: From -channel is not currently recorded in the link data, will need to add it before having multiple output nodes.
  });
}


unsigned int WidgetBlueprint::CountInputLinks(WidgetNode * to_node, fmsynth::Node::Channel to_channel) const
{ // todo: stl algorithm?
  unsigned int count = 0;

  for(auto l : _links)
    if(l)
      if(to_node == l->GetToNode() && to_channel == l->GetToChannel())
        count++;
  
  return count;
}


unsigned int WidgetBlueprint::CountOutputLinks(WidgetNode * from_node, fmsynth::Node::Channel from_channel) const
{
  assert(from_channel == fmsynth::Node::Channel::Form);
  unsigned int count = 0;

  for(auto l : _links)
    if(l)
      if(from_node == l->GetFromNode() && from_channel == fmsynth::Node::Channel::Form)
        count++;
  
  return count;
}


std::shared_ptr<fmsynth::Blueprint> WidgetBlueprint::Build() const
{
  std::lock_guard lock(_blueprint->GetLockMutex());
  assert(CanRun());
  _blueprint->ResetTime();
  return _blueprint;
}



void WidgetBlueprint::Reset()
{
  ProgramPlayer->SetNextProgram(nullptr);

  {
    std::lock_guard lock(_blueprint->GetLockMutex());

    _selected_nodes.clear();
    
    if(!_loading)
      {
        GetMainWindow()->statusBar()->showMessage(QString::fromStdString("New blueprint."), 3000);
        _undopos = 0;
        _undobuffer.clear();
      }

    for(auto l : _links)
      delete l;
    _links.clear();

    for(auto n : _nodes)
      if(n)
        n->deleteLater();
    _nodes.clear();
  }

  _blueprint.reset(new fmsynth::Blueprint);

  SetDirty(false);

  if(!_loading)
    {
      _post_edit_save = to_json();
      GetMainWindow()->UpdateToolbarButtonStates();
    }
}


void WidgetBlueprint::Save(const std::string & filename)
{
  if(filename.empty())
    return;
  SetFilename(filename);
  
  std::ofstream file(_filename);
  if(!file)
    { // todo: Show the error in the GUI.
      std::cerr << "Error while saving: something went wrong\n";
      return;
    }

  auto f = to_json();
  file << f.dump();
  // todo: Check for failure.

  SetDirty(false);
  GetMainWindow()->statusBar()->showMessage(QString::fromStdString("Saved '" + _filename + "'"), 3000);
}


json11::Json WidgetBlueprint::to_json()
{
  UpdateNodesData();
  
  json11::Json::array nodes;
  for(auto n : _nodes)
    if(n)
      nodes.push_back(*n);
  json11::Json::array links;
  for(auto l : _links)
    if(l)
      links.push_back(*l);

  int view_x = 2048;
  int view_y = 2048;
  auto sa = GetScrollArea();
  if(sa)
    {
      view_x = sa->horizontalScrollBar()->value();
      view_y = sa->verticalScrollBar()->value();
    }
  
  return json11::Json::object {
    { "view",  json11::Json::array { view_x, view_y } },
    { "nodes", nodes },
    { "links", links }
  };
}



const std::string & WidgetBlueprint::GetFilename() const
{
  return _filename;
}


void WidgetBlueprint::Load()
{
  auto filename = QFileDialog::getOpenFileName(this, "Open file", QString::fromStdString(_filename), QString("libfmsynth files (*.sbp)")).toStdString();
  if(filename.length() == 0)
    return;
  Load(filename);
}


void WidgetBlueprint::Save()
{
  auto filename = QFileDialog::getSaveFileName(this, "Save file", QString::fromStdString(_filename), QString("libfmsynth files (*.sbp)")).toStdString();
  if(filename.length() == 0)
    return;
  if(!filename.ends_with(".sbp"))
    filename += ".sbp";
  Save(filename);
}


void WidgetBlueprint::SetFilename(const std::string & filename)
{
  _filename = filename;
  auto mwin = GetMainWindow();
  mwin->AddToRecentFiles(filename);
  UpdateWindowTitle();
}

WidgetMainWindow * WidgetBlueprint::GetMainWindow() const
{
  auto mwin = parentWidget();
  while(mwin && mwin->objectName().toStdString() != "MainWindow")
    mwin = mwin->parentWidget();
  assert(mwin);
  return dynamic_cast<WidgetMainWindow *>(mwin);
}



void WidgetBlueprint::Load(const std::string & filename)
{
  assert(filename.empty() == false);
  
  SetFilename(filename);

  GetMainWindow()->statusBar()->showMessage(QString::fromStdString("Loading '" + _filename + "'..."));

  auto [json, error] = fmsynth::util::LoadJsonFile(_filename);
  if(!json || !json->is_object())
    {
      GetMainWindow()->statusBar()->showMessage(QString::fromStdString(error));
      std::cerr << error << std::endl;
      return;
    }

  Load(*json);
  _undopos = 0;
  _undobuffer.clear();
  _post_edit_save = to_json();
  GetMainWindow()->UpdateToolbarButtonStates();
  
  GetMainWindow()->statusBar()->showMessage(QString::fromStdString("Loaded '" + _filename + "'"), 3000);
}


void WidgetBlueprint::Load(const json11::Json & json)
{
  _loading = true;
  _selected_nodes.clear();
  
  assert(json["links"].is_array());
  assert(json["nodes"].is_array());
  auto links = json["links"].array_items();
  auto nodes = json["nodes"].array_items();

  Reset();
  for(auto n : nodes)
    {
      assert(n["node_type"].is_string());
      auto type = n["node_type"].string_value();

      // todo: Set the position in the Node->SetFromJson() instead of doing it here.
      assert(n["position"].is_array());
      assert(n["position"].array_items().size() == 2);
      auto pos_x = n["position"][0].int_value();
      auto pos_y = n["position"][1].int_value();

      auto node = AddNode(pos_x, pos_y, type);
      assert(node);
      node->SetFromJson(n);
    }

  auto FindNodeById = [this](const std::string & node_id) -> WidgetNode *
  {
    for(auto n : _nodes)
      if(n->GetNodeId() == node_id)
        return n;
    assert(false);
    return nullptr;
  };
  
  for(auto l : links)
    {
      auto from_node = FindNodeById(l["from"].string_value());
      auto to_node   = FindNodeById(l["to"].string_value());

      auto channel = fmsynth::Node::StringToChannel(l["to_channel"].string_value());
      AddLink(from_node, to_node, channel);
    }

  _loading = false;
  SetDirty(false);

  auto sa = GetScrollArea();
  sa->horizontalScrollBar()->setValue(json["view"][0].int_value());
  sa->verticalScrollBar()->  setValue(json["view"][1].int_value());
}


void WidgetBlueprint::PostEdit(QWidget * edited_node)
{
  std::set<QWidget *> s { edited_node };
  PostEdit(s);
}


void WidgetBlueprint::PostEdit(const std::set<QWidget *> & edited_nodes)
{
  if(_loading)
    return;
  
  for(auto w : edited_nodes)
    {
      auto en = dynamic_cast<WidgetNode *>(w);
      assert(en);
      if(en)
        en->WidgetToNode();
    }
  PostEdit();
}


void WidgetBlueprint::PostEdit()
{
  if(_loading)
    return;
  
  _undobuffer.resize(_undopos + 1);
  _undobuffer[_undopos] = _post_edit_save;
  _undopos++;

  SetDirty(true);

  _post_edit_save = to_json();
}


void WidgetBlueprint::UpdateWindowTitle()
{
  std::string title;
  if(_dirty)
    title += "* ";
  if(!_filename.empty())
    title += std::filesystem::path(_filename).filename().string() + " - ";
  title += "FMSEdit";

  auto mwin = GetMainWindow();
  mwin->setWindowTitle(QString::fromStdString(title));
}


bool WidgetBlueprint::IsDirty() const
{
  return _dirty;
}


bool WidgetBlueprint::CanRun() const
{
  bool has_start_nodes = false;
  for(auto n : _nodes)
    if(n)
      {
        if(n->GetErrorStatus())
          return false;
        if(dynamic_cast<WidgetNodeConstant *>(n) || dynamic_cast<WidgetNodeGrowth *>(n))
          has_start_nodes = true;
      }
  return has_start_nodes;
}


void WidgetBlueprint::SetDirty(bool dirty)
{
  for(auto n : _nodes)
    if(n)
      n->UpdateErrorStatus();
  NodeSelectionUpdated();
  
  if(_loading)
    return;
  
  _dirty = dirty;

  UpdateWindowTitle();
  GetMainWindow()->UpdateToolbarButtonStates();
}



void WidgetBlueprint::Undo()
{
  if(_undopos > 0)
    {
      if(_undopos >= _undobuffer.size())
        { // Need extra saving here so that the last Redo() has something to load:
          assert(_undopos == _undobuffer.size());
          _undobuffer.resize(_undopos + 1);
          _undobuffer[_undopos] = _post_edit_save;
        }

      _undopos--;
      Load(_undobuffer[_undopos]);
      _post_edit_save = to_json();
      GetMainWindow()->UpdateToolbarButtonStates();
    }
}

void WidgetBlueprint::Redo()
{
  if(_undopos < _undobuffer.size())
    {
      _undopos++;
      Load(_undobuffer[_undopos]);
      _post_edit_save = to_json();
      GetMainWindow()->UpdateToolbarButtonStates();
    }
}


std::vector<Link *> WidgetBlueprint::GetInputLinks(const WidgetNode * to_node, fmsynth::Node::Channel to_channel) const
{
  std::vector<Link *> rv;
  for(auto link : _links)
    if(link)
      if(to_node == link->GetToNode() && to_channel == link->GetToChannel())
        rv.push_back(link);
  
  return rv;
}


std::vector<Link *> WidgetBlueprint::GetOutputLinks(const WidgetNode * from_node, fmsynth::Node::Channel from_channel) const
{
  assert(from_channel == fmsynth::Node::Channel::Form);
  std::vector<Link *> rv;
  for(auto link : _links)
    if(link)
      if(from_node == link->GetFromNode() && from_channel == fmsynth::Node::Channel::Form)
        rv.push_back(link);
  
  return rv;
}


bool WidgetBlueprint::CanUndo() const
{
  return _undopos > 0;
}


bool WidgetBlueprint::CanRedo() const
{
  return _undopos + 1 < _undobuffer.size();
}


unsigned int WidgetBlueprint::GetSelectedNodesCount() const
{
  return static_cast<unsigned int>(_selected_nodes.size());
}


void WidgetBlueprint::NodeSelectionAll()
{
  for(auto n : _nodes)
    if(n)
      NodeSelectionAdd(n);
}


void WidgetBlueprint::NodeSelectionAdd(WidgetNode * node)
{
  node->setProperty("real_position", node->pos());
  _selected_nodes.insert(node);
  NodeSelectionUpdated();
}


void WidgetBlueprint::NodeSelectionToggle(WidgetNode * node)
{
  if(_selected_nodes.contains(node))
    _selected_nodes.erase(node);
  else
    {
      node->setProperty("real_position", node->pos());
      _selected_nodes.insert(node);
    }
  NodeSelectionUpdated();
}


void WidgetBlueprint::NodeSelectionSet(WidgetNode * node)
{
  if(_selected_nodes.contains(node))
    return;
  
  _selected_nodes.clear();
  if(node)
    {
      node->setProperty("real_position", node->pos());
      _selected_nodes.insert(node);
    }
  NodeSelectionUpdated();
}


void WidgetBlueprint::NodeSelectionUpdated()
{
  for(auto n : _nodes)
    if(n)
      n->SetSelectedVisuals(_selected_nodes.contains(n));
  GetMainWindow()->UpdateToolbarButtonStates();
}


void WidgetBlueprint::DeleteSelectedNodes()
{
  for(auto n : _selected_nodes)
    DeleteNode(n);
  
  _selected_nodes.clear();
  NodeSelectionUpdated();
}


void WidgetBlueprint::MoveSelectedNodes(const QPoint & delta)
{
  auto d = delta;
  for(auto node : _selected_nodes)
    if(node)
      {
        auto p = node->pos() + d;
        if(p.x() < 0)
          d.setX(d.x() + std::abs(p.x()));
        else if(p.x() > 4096 - 100)
          d.setX(d.x() - (p.x() - (4096 - 100)));
        if(p.y() < 0)
          d.setY(d.y() + std::abs(p.y()));
        else if(p.y() > 4096 - 100)
          d.setY(d.y() - (p.y() - (4096 - 100)));
      }
  
  for(auto node : _selected_nodes)
    if(node)
      {
        auto pos = node->property("real_position").toPoint();
        pos += d;
        node->setProperty("real_position", pos);
        
        if(_snap_to_grid)
          {
            auto snap = [](int x) { return x - x % 20; };
            pos.setX(snap(pos.x()));
            pos.setY(snap(pos.y()));
          }
        
        node->move(pos);
        PostEdit(node);
        UpdateLinks(node);
      }
}


std::vector<WidgetNode *> WidgetBlueprint::GetOverlappingNodes(const WidgetNode * for_node) const
{
  auto r = for_node->rect();
  r.setTopLeft(    for_node->mapTo(this, r.topLeft()));
  r.setBottomRight(for_node->mapTo(this, r.bottomRight()));

  std::vector<WidgetNode *> rv;
  for(auto node : _nodes)
    if(node && node != for_node)
      {
        auto p1 = node->mapTo(this, node->rect().topLeft());
        auto p2 = node->mapTo(this, node->rect().bottomRight());
        if(r.contains(p1) || r.contains(p2))
          rv.push_back(node);
      }
  return rv;
}


void WidgetBlueprint::UpdateNodesData()
{ // todo: This should be automatic?
  for(auto n : _nodes)
    if(n)
      n->WidgetToNode();
}


std::shared_ptr<fmsynth::Blueprint> WidgetBlueprint::GetBlueprint() const
{
  return _blueprint;
}


void WidgetBlueprint::SetSnapToGrid(bool snap_to_grid)
{
  _snap_to_grid = snap_to_grid;
  for(auto node : _nodes)
    node->setProperty("real_position", node->pos());
}

