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
#include "Blueprint.hh"
#include "Link.hh"
#include "Node.hh"
#include "WidgetBlueprint.hh"
#include "WidgetEditNodeProperties.hh"
#include "WidgetRectangle.hh"
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include <cassert>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QScrollArea>
#include "QtIncludeEnd.hh"


WidgetNode::WidgetNode(QWidget * parent, std::shared_ptr<fmsynth::Node> node, bool takes_input, bool has_output)
  : QWidget(parent),
    _ui_node(new Ui::Node),
    _click_to_raise(true),
    _selecting_selects_contents(false),
    _stack_under(nullptr),
    _node(node),
    _node_type("Unknown"),
    _editor_only_node_id(std::to_string(reinterpret_cast<intptr_t>(this))),
    _multi_input_amplitude(false),
    _multi_input_form(false),
    _multi_input_aux(false),
    _error_status(false),
    _error_widget(nullptr),
    _selected_widget(nullptr)
{
  _ui_node->setupUi(this);
  _ui_node->_title->installEventFilter(this);

  _ui_node->_input_amplitude->setVisible(takes_input);
  _ui_node->_input_amplitude->SetIsOptional();
  _ui_node->_input_form->setVisible(takes_input);
  _ui_node->_input_aux->setVisible(false);
  _ui_node->_input_aux->SetIsOptional();
  _ui_node->_output->setVisible(has_output);
  _ui_node->_output->SetOwner(GetWidgetBlueprint(), this);
  _ui_node->_output->SetIsOptional();

  connect(_ui_node->_closebutton, &QPushButton::clicked,
          [this]([[maybe_unused]] bool checked)
          {
            GetWidgetBlueprint()->DeleteNode(this);
          });

  connect(_ui_node->_enabled, &QCheckBox::stateChanged,
          [this]([[maybe_unused]] int state)
          {
            if(_node)
              {
                auto bp = GetWidgetBlueprint()->GetBlueprint();
                std::lock_guard lock(bp->GetLockMutex());
                _node->SetEnabled(bp->GetRoot(), _ui_node->_enabled->isChecked());
              }
          });

  connect(_ui_node->_propertiesbutton, &QPushButton::clicked,
          [this]([[maybe_unused]] bool checked)
          {
            auto dialog = new WidgetEditNodeProperties(this);
            dialog->show();
          });
  
  ListenWidgetChanges({ _ui_node->_enabled });
}


WidgetNode::~WidgetNode()
{
  delete _ui_node;
  if(_error_widget)
    _error_widget->deleteLater();
  if(_selected_widget)
    _selected_widget->deleteLater();
}


fmsynth::Node * WidgetNode::GetNode() const
{
  return _node.get();
}


std::shared_ptr<fmsynth::Node> WidgetNode::GetSharedNode() const
{
  return _node;
}


void WidgetNode::AddAuxInput()
{
  _ui_node->_input_aux->setVisible(true);
}


bool WidgetNode::eventFilter(QObject * object, QEvent * event)
{
  bool rv = false;
  auto widget = dynamic_cast<QWidget *>(object);

  if(object == _ui_node->_title)
    {
      auto me = static_cast<QMouseEvent *>(event);
      if(event->type() == QEvent::MouseButtonPress)
        {
          auto blueprint = GetWidgetBlueprint();
          if(_selecting_selects_contents)
            {
              for(auto node : blueprint->GetOverlappingNodes(this))
                if(node && !node->_selecting_selects_contents)
                  blueprint->NodeSelectionAdd(node);
            }

          if(me->modifiers() & Qt::ControlModifier)
            blueprint->NodeSelectionToggle(this);
          else if(me->modifiers() & Qt::ShiftModifier)
            blueprint->NodeSelectionAdd(this);
          else
            blueprint->NodeSelectionSet(this);

          assert(widget);
          _drag_last_time = std::chrono::steady_clock::now();
          _drag_last_pos = widget->mapTo(blueprint, me->pos());
          if(_click_to_raise)
            {
              if(_stack_under)
                stackUnder(_stack_under);
              else
                raise();
            }
          rv = true;
        }
      else if(event->type() == QEvent::MouseMove)
        {
          if(me->buttons() & Qt::LeftButton)
            {
              assert(widget);
              auto blueprint = GetWidgetBlueprint();
              auto pos = widget->mapTo(blueprint, me->pos());
              auto delta = pos - _drag_last_pos;
              _drag_last_pos = pos;
              
              blueprint->MoveSelectedNodes(delta);
              
              auto now = std::chrono::steady_clock::now();
              std::chrono::duration<double> passed = now - _drag_last_time;
              if(passed.count() > 0.01)
                { // todo: Perform this scrolling also when the mouse does not move to fix the currently slightly annoying "jerky" movement.
                  _drag_last_time = now;
                  blueprint->GetScrollArea()->ensureVisible(pos.x(), pos.y(), 100, 100);
                }
              rv = true;
            }
        }
    }
  return rv;
}


const std::string & WidgetNode::GetNodeId() const
{
  auto node = GetNode();
  if(node)
    return node->GetId();
  else
    return _editor_only_node_id;
}


void WidgetNode::SetNodeId(const std::string & id)
{
  auto node = GetNode();
  if(node)
    node->SetId(id);
  else
    _editor_only_node_id = id;
  UpdateNodeId();
}


void WidgetNode::UpdateNodeId()
{
  auto id = GetNodeId();
  auto qid = QString::fromStdString(id);
  _ui_node->_output->         setProperty("NodeId", qid);
  _ui_node->_input_amplitude->setProperty("NodeId", qid);
  _ui_node->_input_form->     setProperty("NodeId", qid);
  _ui_node->_input_aux->      setProperty("NodeId", qid);

  _ui_node->_title->setToolTip(QString::fromStdString(id + " (" + _node_type + ")"));
}


WidgetBlueprint * WidgetNode::GetWidgetBlueprint() const
{
  auto blueprint = parentWidget();
  while(blueprint && blueprint->objectName().toStdString() != "_blueprint")
    blueprint = blueprint->parentWidget();
  assert(blueprint);
  return dynamic_cast<WidgetBlueprint *>(blueprint);
}


void WidgetNode::SetIsMultiInput(fmsynth::Node::Channel channel)
{
  QWidget * w = nullptr;
  switch(channel)
    {
    case fmsynth::Node::Channel::Amplitude:
      _multi_input_amplitude = true;
      w = _ui_node->_input_amplitude;
      break;
    case fmsynth::Node::Channel::Form:
      _multi_input_form = true;
      w = _ui_node->_input_form;
      break;
    case fmsynth::Node::Channel::Aux:
      _multi_input_aux = true;
      w = _ui_node->_input_aux;
      break;
    default:
      assert(false);
    }
  w->setToolTip("Values in (1 or more)");
}


bool WidgetNode::IsMultiInput(fmsynth::Node::Channel channel)
{
  switch(channel)
    {
    case fmsynth::Node::Channel::Amplitude: return _multi_input_amplitude;
    case fmsynth::Node::Channel::Form:      return _multi_input_form;
    case fmsynth::Node::Channel::Aux:       return _multi_input_aux;
    }
  assert(false);
  return false;
}


void WidgetNode::SetConnectorsRanges()
{
  auto r2s = [](fmsynth::Input::Range r) -> std::string {
    switch(r)
      {
      case fmsynth::Input::Range::Inf_Inf:      return "Inf";
      case fmsynth::Input::Range::MinusOne_One: return "-1_1";
      case fmsynth::Input::Range::Zero_One:     return "0_1";
      }
    assert(false);
    return "";
  };

  auto node = GetNode();
  auto r = node->GetFormOutputRange();
  _ui_node->_output->SetRangeFilename(r2s(r));

  r = node->GetInputRange(fmsynth::Node::Channel::Amplitude);
  _ui_node->_input_amplitude->SetRangeFilename(r2s(r));

  r = node->GetInputRange(fmsynth::Node::Channel::Form);
  _ui_node->_input_form->SetRangeFilename(r2s(r));

  r = node->GetInputRange(fmsynth::Node::Channel::Aux);
  _ui_node->_input_aux->SetRangeFilename(r2s(r));
}


void WidgetNode::SetConnectorsRangesRecursively()
{
  SetConnectorsRanges();

  auto bp = GetWidgetBlueprint();
  for(auto l : bp->GetOutputLinks(this, fmsynth::Node::Channel::Form))
    l->GetToNode()->SetConnectorsRangesRecursively();
}


void WidgetNode::UpdateConnectorStates()
{
  auto bp = GetWidgetBlueprint();
  
  auto amplitude_input_count = bp->CountInputLinks(this, fmsynth::Node::Channel::Amplitude);
  auto form_input_count      = bp->CountInputLinks(this, fmsynth::Node::Channel::Form);
  auto aux_input_count       = bp->CountInputLinks(this, fmsynth::Node::Channel::Aux);
  auto output_count          = bp->CountOutputLinks(this, fmsynth::Node::Channel::Form);
  
  _ui_node->_input_amplitude->SetIsConnected(amplitude_input_count > 0);
  _ui_node->_input_form->SetIsConnected(form_input_count > 0);
  _ui_node->_input_aux->SetIsConnected(aux_input_count > 0);
  _ui_node->_output->SetIsConnected(output_count > 0);

  SetConnectorsRangesRecursively();
}


json11::Json WidgetNode::to_json() const
{
  auto rv = json11::Json::object {
    { "position",  json11::Json::array { pos().x(), pos().y() } }
  };
  
  auto node = GetNode();
  if(node)
    {
      auto items = node->to_json().object_items();
      for(auto & [k, v] : items)
        rv[k] = v;
    }

  // Overwrite whatever the Node set:
  rv["node_type"] = _node_type;
  
  return rv;
}


void WidgetNode::NodeToWidget()
{
  UpdateNodeId();
}


void WidgetNode::WidgetToNode()
{
}


void WidgetNode::SetFromJson(const json11::Json & json)
{
  auto cs = Qt::Unchecked;
  
  auto node = GetNode();
  if(node)
    {
      node->SetFromJson(json);

      if(node->IsEnabled())
        cs = Qt::Checked;
    }
  
  _ui_node->_enabled->setCheckState(cs);

  NodeToWidget();
}


void WidgetNode::SetNodeType(const std::string & type, const std::string & icon_filename)
{
  _node_type = type;
  QIcon icon;
  std::string iname("Node");
  if(icon_filename.empty())
    iname += type;
  else
    iname += icon_filename;

  icon.addFile(QString::fromStdString("images/" + iname + ".png"), QSize(), QIcon::Normal, QIcon::Off);
  _ui_node->_title->setIcon(icon);
  _ui_node->_title->setText(QString::fromStdString(type));
}


WidgetConnector * WidgetNode::GetInputWidgetConnector(fmsynth::Node::Channel channel)
{
  switch(channel)
    {
    case fmsynth::Node::Channel::Amplitude: return _ui_node->_input_amplitude;
    case fmsynth::Node::Channel::Form:      return _ui_node->_input_form;
    case fmsynth::Node::Channel::Aux:       return _ui_node->_input_aux;
    }
  assert(false);
  return nullptr;
}


WidgetConnector * WidgetNode::GetOutputWidgetConnector(fmsynth::Node::Channel channel)
{
  assert(channel == fmsynth::Node::Channel::Form);
  return _ui_node->_output;
}



void WidgetNode::ListenWidgetChanges(const std::vector<QWidget *> & widgets)
{
  auto Listen = [this](QWidget * widget) -> void
  {
      if(auto w = dynamic_cast<QComboBox *>(widget))
        {
          connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]([[maybe_unused]] int index) { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QLineEdit *>(widget))
        {
          connect(w, &QLineEdit::editingFinished, [this]() { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QDoubleSpinBox *>(widget))
        {
          connect(w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this]([[maybe_unused]] double d) { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QAbstractSpinBox *>(widget))
        {
          connect(w, &QAbstractSpinBox::editingFinished, [this]() { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QAbstractSlider *>(widget))
        {
          connect(w, &QAbstractSlider::valueChanged, [this]([[maybe_unused]] int value) { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QPushButton *>(widget))
        {
          connect(w, &QPushButton::clicked, [this]([[maybe_unused]] bool checked) { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      if(auto w = dynamic_cast<QCheckBox *>(widget))
        {
          connect(w, &QCheckBox::stateChanged, [this]([[maybe_unused]] int state) { GetWidgetBlueprint()->PostEdit(this); });
          return;
        }
      assert(false);
  };    
  
  for(auto widget : widgets)
    Listen(widget);
}



const std::string & WidgetNode::GetNodeType() const
{
  return _node_type;
}


bool WidgetNode::GetErrorStatus() const
{
  auto node = GetNode();
  if(node && node->IsEnabled())
    {
      std::array<WidgetConnector *, 4> cons {
        _ui_node->_input_amplitude,
        _ui_node->_input_form,
        _ui_node->_input_aux,
        _ui_node->_output
      };
      for(auto c : cons)
        if(!c->IsOptional())
          if(!c->IsConnected())
            return true;
    }

  return false;
}


void WidgetNode::UpdateErrorStatus()
{
  _error_status = GetErrorStatus();

  if(_error_widget)
    _error_widget->deleteLater();
  _error_widget = nullptr;
  if(_error_status)
    {
      QPoint bordersize(3, 3);
      auto topleft     = mapToParent(rect().topLeft()) - bordersize;
      auto bottomright = mapToParent(rect().bottomRight()) + bordersize;
      _error_widget = new WidgetRectangle(GetWidgetBlueprint(), topleft, bottomright);
      _error_widget->show();
      _error_widget->setDisabled(true);
      _error_widget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
      _error_widget->stackUnder(this);
    }
}


void WidgetNode::resizeEvent(QResizeEvent * event)
{
  QWidget::resizeEvent(event);
  // Update _error_widget and _selected_widget to fit the new size:
  if(_error_widget)
    UpdateErrorStatus();
  if(_selected_widget)
    SetSelectedVisuals(true);
}


bool WidgetNode::DependsOn(const WidgetNode * node) const
{
  std::vector<const WidgetNode *> work;

  auto blueprint = GetWidgetBlueprint();

  work.push_back(this);
  while(work.size() > 0)
    {
      auto cur = work.back();
      work.pop_back();

      if(cur == node)
        return true;

      std::array channels { fmsynth::Node::Channel::Amplitude, fmsynth::Node::Channel::Form, fmsynth::Node::Channel::Aux };
      for(auto channel : channels)
        for(auto link : blueprint->GetInputLinks(cur, channel))
          work.push_back(link->GetFromNode());
    }
  return false;
}


void WidgetNode::SetSelectedVisuals(bool is_selected)
{
  if(_selected_widget)
    _selected_widget->deleteLater();
  _selected_widget = nullptr;
  
  if(is_selected)
    {
      int bs = _error_widget ? 4 : 2;
      QPoint bordersize(bs, bs);
      auto topleft     = mapToParent(rect().topLeft()) - bordersize;
      auto bottomright = mapToParent(rect().bottomRight()) + bordersize;
      auto w = new WidgetRectangle(GetWidgetBlueprint(), topleft, bottomright);
      w->SetColor(QColor::fromRgbF(0.7, 0.7, 1.0, 1.0));
      w->show();
      w->setDisabled(true);
      w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
      if(_error_widget)
        w->stackUnder(_error_widget);
      else
        w->stackUnder(this);
      _selected_widget = w;
    }
}
