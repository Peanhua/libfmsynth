#ifndef WIDGET_NODE_HH_
#define WIDGET_NODE_HH_
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
#include "Ui.hh"
#include <json11.hpp>
#include "QtIncludeBegin.hh"
#include <QtWidgets/QWidget>
#include <QtCore/QPoint>
#include "QtIncludeEnd.hh"

class WidgetBlueprint;
class WidgetConnector;


class WidgetNode : public QWidget
{
public:
  WidgetNode(QWidget * parent, std::shared_ptr<fmsynth::Node> node, bool takes_input, bool has_output);
  virtual ~WidgetNode();

  [[nodiscard]] bool eventFilter(QObject * object, QEvent * event) override;
  void               resizeEvent(QResizeEvent * event)             override;

  [[nodiscard]] bool                 DependsOn(const WidgetNode * node) const;

  [[nodiscard]] fmsynth::Node *      GetNode()     const;
  [[nodiscard]] std::shared_ptr<fmsynth::Node> GetSharedNode() const;
  [[nodiscard]] const std::string &  GetNodeType() const;
  [[nodiscard]] const std::string &  GetNodeId()   const;
  void                               SetNodeId(const std::string & id);
  void                               UpdateNodeId();
  [[nodiscard]] WidgetBlueprint *    GetWidgetBlueprint() const;
  [[nodiscard]] bool                 IsMultiInput(fmsynth::Node::Channel channel);
  [[nodiscard]] WidgetConnector *    GetInputWidgetConnector(fmsynth::Node::Channel channel);
  [[nodiscard]] WidgetConnector *    GetOutputWidgetConnector(fmsynth::Node::Channel channel);

  void                               SetSelectedVisuals(bool is_selected);
  
  [[nodiscard]] virtual bool         GetErrorStatus() const;
  void                               UpdateErrorStatus();

  virtual void                       UpdateConnectorStates();

  virtual void                       NodeToWidget();
  virtual void                       WidgetToNode();

  [[nodiscard]] virtual json11::Json to_json() const;
  virtual void                       SetFromJson(const json11::Json & json);

protected:
  Ui::Node *      _ui_node;
  bool            _click_to_raise;
  bool            _selecting_selects_contents;
  QWidget *       _stack_under;

  void AddAuxInput();
  void SetNodeType(const std::string & type, const std::string & icon_filename = "");
  void SetIsMultiInput(fmsynth::Node::Channel channel);
  void SetConnectorsRanges();
  void SetConnectorsRangesRecursively();

  void ListenWidgetChanges(const std::vector<QWidget *> & widgets);
  
private:
  std::shared_ptr<fmsynth::Node> _node;
  std::string _node_type;
  std::string _editor_only_node_id;
  QPoint _drag_last_pos;
  std::chrono::time_point<std::chrono::steady_clock> _drag_last_time;
  bool   _multi_input_amplitude;
  bool   _multi_input_form;
  bool   _multi_input_aux;
  bool      _error_status;
  QWidget * _error_widget;
  QWidget * _selected_widget;
};


#endif
