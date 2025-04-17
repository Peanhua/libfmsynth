/*
  libfmsynth
  Copyright (C) 2021-2025  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetInputConstant.hh"
#include "QtIncludeBegin.hh"
#include <QtWidgets/QHBoxLayout>
#include "QtIncludeEnd.hh"


WidgetInputConstant::WidgetInputConstant(QWidget * parent)
  : QWidget(parent)
{
  auto layout = new QHBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 2);
  
  _value = new QDoubleSpinBox(this);
  QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(_value->sizePolicy().hasHeightForWidth());
  _value->setSizePolicy(sizePolicy);
  _value->setMinimumSize(QSize(100, 0));
  _value->setDecimals(3);
  _value->setMinimum(-100000);
  _value->setMaximum(100000);
  _value->setSingleStep(0.1);
  _value->setValue(1);
  
  layout->addWidget(_value);

  _value_type = new QComboBox(this);
  _value_type->addItem("Absolute");        _units.push_back(fmsynth::ConstantValue::Unit::Absolute);
  _value_type->addItem("Hertz (Hz)");      _units.push_back(fmsynth::ConstantValue::Unit::Hertz);
  _value_type->addItem("Pi (\317\200)");   _units.push_back(fmsynth::ConstantValue::Unit::Pi);
  _value_type->addItem("Euler (\u2107)");  _units.push_back(fmsynth::ConstantValue::Unit::E);

  layout->addWidget(_value_type);

  _control_widgets.push_back(_value);
  _control_widgets.push_back(_value_type);
}


const std::vector<QWidget *> & WidgetInputConstant::GetControlWidgets() const
{
  return _control_widgets;
}


void WidgetInputConstant::SetConstantValue(const fmsynth::ConstantValue & value)
{
  _value->setValue(value.GetUnitValue());
  auto it = std::find(_units.cbegin(), _units.cend(), value.GetUnit());
  assert(it != _units.cend());
  _value_type->setCurrentIndex(static_cast<int>(it - _units.cbegin()));
}


const fmsynth::ConstantValue WidgetInputConstant::GetConstantValue() const
{
  return fmsynth::ConstantValue {
    _value->value(),
    _units[static_cast<unsigned int>(_value_type->currentIndex())]
  };
}

