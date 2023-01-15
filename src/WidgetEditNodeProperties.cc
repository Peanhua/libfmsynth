/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetEditNodeProperties.hh"
#include "Blueprint.hh"
#include "Node.hh"
#include "WidgetBlueprint.hh"
#include "WidgetNode.hh"
#include <cassert>
#include "QtIncludeBegin.hh"
#include "UiEditNodeProperties.hh"
#include <QValidator>
#include "QtIncludeEnd.hh"

#include <iostream>
class IdValidator : public QValidator
{
public:
  IdValidator(QObject * parent, const std::vector<std::string> & existing_ids)
    : QValidator(parent),
      _existing_ids(existing_ids)
  {
  }

  QValidator::State validate(QString & input, [[maybe_unused]] int & pos) const override
  {
    if(input.toStdString().empty())
      return QValidator::State::Invalid;
    if(std::find(_existing_ids.cbegin(), _existing_ids.cend(), input.toStdString()) != _existing_ids.cend())
      return QValidator::State::Invalid;
    return QValidator::State::Acceptable;
  }

private:
  std::vector<std::string> _existing_ids;
};


WidgetEditNodeProperties::WidgetEditNodeProperties(QWidget * parent)
  : QDialog(parent),
    _ui(new Ui::EditNodeProperties)
{
  _ui->setupUi(this);

  _nodewidget = dynamic_cast<WidgetNode *>(parent);
  assert(_nodewidget);
  _ui->_type->setText(QString::fromStdString(_nodewidget->GetNodeType()));
  _ui->_id->setText(QString::fromStdString(_nodewidget->GetNodeId()));
  _ui->_id->setValidator(new IdValidator(_ui->_id, GetExistingIds()));
  auto node = _nodewidget->GetNode();
  if(node)
    {
      auto cs = _nodewidget->GetNode()->IsEnabled() ? Qt::Checked : Qt::Unchecked;
      _ui->_enabled->setCheckState(cs);
    }
  else
    {
      _ui->_enabled->hide();
      _ui->_enabled_label->hide();
    }

  connect(this, &QDialog::accepted,
          [this]()
          {
            ApplyChanges();
          });

  adjustSize();
}


void WidgetEditNodeProperties::ApplyChanges()
{
  assert(_nodewidget);
  if(!_nodewidget)
    return;

  _nodewidget->SetNodeId(_ui->_id->text().toStdString());
  
  auto node = _nodewidget->GetNode();
  if(node)
    {
      auto bp = _nodewidget->GetWidgetBlueprint()->GetBlueprint();
      std::lock_guard lock(bp->GetLockMutex());
      node->SetEnabled(bp->GetRoot(), _ui->_enabled->isChecked());
    }

  _nodewidget->GetWidgetBlueprint()->SetDirty(true);
}


std::vector<std::string> WidgetEditNodeProperties::GetExistingIds() const
{
  std::vector<std::string> ids;
  for(auto node : _nodewidget->GetWidgetBlueprint()->GetBlueprint()->GetAllNodes())
    if(node)
      ids.push_back(node->GetId());
  return ids;
}
