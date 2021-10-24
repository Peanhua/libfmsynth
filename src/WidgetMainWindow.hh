#ifndef USER_INTERFACE_QT_HH_
#define USER_INTERFACE_QT_HH_
/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "Ui.hh"
#include <set>
#include <vector>
#include "QtIncludeBegin.hh"
#include <QtWidgets/QMainWindow>
#include "QtIncludeEnd.hh"


class WidgetMainWindow : public QMainWindow
{
public:
  WidgetMainWindow(QWidget * parent = nullptr);

  void AddToRecentFiles(const std::string & filename);
  void ClearRecentFiles();

  void UpdateToolbarButtonStates();

  QWidget * GetCommentBorderWidget();
  
  void closeEvent(QCloseEvent * event) override;

private:
  Ui::MainWindow * _ui;
  QPalette         _default_palette;
  std::vector<std::string> _recent_files;
  std::vector<QAction *>   _recent_files_actions;
  
  void BuildAndPlay();
  void ThemeDefault();
  void ThemeDark();
  void ToggleCategoryLayout(const std::string & category);
  void ToggleCategoryExpand(const std::string & category);
  void HelpAbout();
  bool AskUserConfirmation();
};

#endif
