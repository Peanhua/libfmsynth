/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetConnector.hh"


WidgetConnector::WidgetConnector(QWidget * parent)
  : QPushButton(parent),
    _is_input(false),
    _is_optional(false)
{
  setFlat(true);
}


void WidgetConnector::SetIsOptional()
{
  _is_optional = true;
  SetIsConnected(_connected);
}


void WidgetConnector::SetRangeFilename(const std::string & range_filename)
{
  _range_filename = range_filename;
  SetIsConnected(_connected);
}


void WidgetConnector::SetIsConnected(bool is_connected)
{
  _connected = is_connected;
  
  std::string filename("images/Link");
  if(_connected)
    {
#if 1
      filename += "Green"; // Connected
#else
      if(_is_input)
        filename += "Green"; // Connected input
      else
        filename += "Blue"; // Connected output
#endif
    }
  else
    {
      if(_is_optional)
        filename += "Gray"; // Not connected optional input or output
      else if(_is_input)
        filename += "Red"; // Not connected mandatory input
      else
        filename += "Yellow"; // Not connected mandatory output
    }

  QIcon icon;
  icon.addFile(QString::fromStdString(filename + _range_filename + ".png"), QSize(), QIcon::Normal, QIcon::Off);
  setIcon(icon);
  setIconSize(size());
}


bool WidgetConnector::IsOptional() const
{
  return _is_optional;
}


bool WidgetConnector::IsConnected() const
{
  return _connected;
}
