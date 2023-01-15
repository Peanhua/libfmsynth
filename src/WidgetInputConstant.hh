#ifndef WIDGET_INPUT_CONSTANT_HH_
#define WIDGET_INPUT_CONSTANT_HH_
/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "ConstantValue.hh"
#include "QtIncludeBegin.hh"
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QWidget>
#include "QtIncludeEnd.hh"


class WidgetInputConstant : public QWidget
{
public:
  WidgetInputConstant(QWidget * parent);

  [[nodiscard]] const std::vector<QWidget *> & GetControlWidgets() const;

  void                                       SetConstantValue(const fmsynth::ConstantValue & value);
  [[nodiscard]] const fmsynth::ConstantValue GetConstantValue() const;
  
private:
  QDoubleSpinBox *       _value;
  QComboBox *            _value_type;
  std::vector<QWidget *> _control_widgets;
  std::vector<fmsynth::ConstantValue::Unit> _units;
};

#endif
