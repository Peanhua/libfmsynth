/*
  libfmsynth
  Copyright (C) 2021-2022  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif
#include "WidgetMainWindow.hh"
#include "Blueprint.hh"
#include "Node.hh"
#include "Player.hh"
#include "RtAudio.hh"
#include "Settings.hh"
#include "StdFormat.hh"
#include "WidgetBlueprint.hh"
#include "WidgetHelpAbout.hh"
#include "QtIncludeBegin.hh"
#include "UiMainWindow.hh"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include "QtIncludeEnd.hh"
#include <cassert>
#include <iostream>


WidgetMainWindow::WidgetMainWindow(QWidget * parent, const char * file_to_open)
  : QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
  _ui->setupUi(this);
  _default_palette = palette();

  assert(ProgramPlayer);
  {
    const auto rates = ProgramPlayer->GetAudioDevice()->GetSampleRates();
    assert(rates.size() > 0);
    unsigned int currate = static_cast<unsigned int>(UserSettings->GetInt("sample_rate"));
    if(std::find(rates.cbegin(), rates.cend(), currate) == rates.cend())
      currate = *std::max_element(rates.cbegin(), rates.cend());

    auto ag = new QActionGroup(_ui->_menu_settings_samplerate);
    for(auto rate : rates)
      {
        auto action = new QAction(this);
        action->setObjectName(QString::fromStdString(std::string("actionSampleRate") + std::to_string(rate)));
        action->setCheckable(true);
        if(rate == currate)
          action->setChecked(true);
        action->setText(QCoreApplication::translate("MainWindow", std::to_string(rate).c_str(), nullptr));
        _ui->_menu_settings_samplerate->addAction(action);
        ag->addAction(action);
      }
    ag->setExclusive(true);
  }
  {
    const auto & names = ProgramPlayer->GetAudioDevice()->GetDeviceNames();
    auto default_id = ProgramPlayer->GetAudioDevice()->GetDefaultDeviceId();

    std::vector<std::string> devices;
    devices.push_back(format("DEFAULT: {}", names[default_id]));
    for(auto d : names)
      devices.push_back(d);
    
    auto ag = new QActionGroup(_ui->_menu_settings_playback_device);
    int id = -1;
    for(auto name : devices)
      {
        if(!name.empty())
          {
            auto action = new QAction(this);
            action->setObjectName(QString::fromStdString(std::string("actionPlaybackDevice") + std::to_string(id)));
            action->setCheckable(true);
            if(id == UserSettings->GetInt("playback_device"))
              action->setChecked(true);
            action->setText(QCoreApplication::translate("MainWindow", name.c_str(), nullptr));
            _ui->_menu_settings_playback_device->addAction(action);
            ag->addAction(action);
          }
        id++;
      }
    ag->setExclusive(true);
  }
  
  std::vector<std::string> cats {
    "inputs",
    "oscillators",
    "envelopes",
    "filters",
    "math",
    "outputs",
    "miscellaneous"
  };
  for(auto category : cats)
    {
      // Save button labels:
      auto root = _ui->_addnodes->findChild<QWidget *>(QString::fromStdString("_category_" + category), Qt::FindChildrenRecursively);
      assert(root);
      for(auto b : root->children())
        {
          auto button = dynamic_cast<QPushButton *>(b);
          if(button)
            button->setProperty("button_label", button->text());
        }

      // Setup button callbacks:
      auto layoutbutton = _ui->_addnodes->findChild<QPushButton *>(QString::fromStdString("_category_layout_" + category), Qt::FindChildrenRecursively);
      connect(layoutbutton, &QPushButton::clicked,
              [this, category]([[maybe_unused]] bool checked) { ToggleCategoryLayout(category); });

      auto expandbutton = _ui->_addnodes->findChild<QPushButton *>(QString::fromStdString("_category_expand_" + category), Qt::FindChildrenRecursively);
      connect(expandbutton, &QPushButton::clicked,
              [this, category]([[maybe_unused]] bool checked) { ToggleCategoryExpand(category); });
    }

  connect(_ui->_menu, &QMenuBar::triggered,
          [this](QAction * action)
          {
            auto a = action->objectName().toStdString();
            if(a == "actionMenuQuit")
              {
                if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
                  {
                    SaveWindowSettings();
                    QApplication::quit();
                  }
              }
            else if(a == "actionMenuNew")
              {
                if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
                  {
                    _ui->_blueprint->Reset();
                    _ui->_blueprint->SetFilename("");
                  }
              }
            else if(a == "actionMenuSave")
              {
                if(_ui->_blueprint->GetFilename().empty())
                  _ui->_blueprint->Save();
                else
                  _ui->_blueprint->Save(_ui->_blueprint->GetFilename());
              }
            else if(a == "actionMenuSaveAs")
              _ui->_blueprint->Save();
            else if(a == "actionMenuOpen")
              {
                if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
                  _ui->_blueprint->Load();
              }
            else if(a == "actionMenuRevert")
              {
                if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
                  _ui->_blueprint->Load(_ui->_blueprint->GetFilename());
              }
            else if(a.starts_with("actionMenuRecent:"))
              {
                if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
                  _ui->_blueprint->Load(a.substr(std::string("actionMenuRecent:").length()));
              }
            else if(a == "actionMenuClearHistory")
              ClearRecentFiles();
            else if(a == "actionMenuPlay" || a == "actionMenuRestart")
              BuildAndPlay();
            else if(a == "actionMenuStop")
              {
                ProgramPlayer->SetNextProgram(nullptr);
                UpdateToolbarButtonStates();
                auto bp = _ui->_blueprint->GetBlueprint();
                if(bp)
                  bp->SetIsFinished();
              }
            else if(a == "actionMenuThemeDefault")
              ThemeDefault();
            else if(a == "actionMenuThemeDark")
              ThemeDark();
            else if(a == "actionMenuHelpAbout")
              HelpAbout();
            else if(a == "actionUndo")
              _ui->_blueprint->Undo();
            else if(a == "actionRedo")
              _ui->_blueprint->Redo();
            else if(a == "actionDelete")
              _ui->_blueprint->DeleteSelectedNodes();
            else if(a == "actionSelectAll")
              _ui->_blueprint->NodeSelectionAll();
            else if(a == "actionSelectNone")
              _ui->_blueprint->NodeSelectionSet(nullptr);
            else if(a == "actionSnapToGrid")
              {
                bool snapping = _ui->actionSnapToGrid->isChecked();
                _ui->_blueprint->SetSnapToGrid(snapping);
                UserSettings->Set("snap_to_grid", snapping);
              }
            else if(a.starts_with("actionSampleRate"))
              {
                auto wasplaying = ProgramPlayer->IsPlaying();
                ProgramPlayer->SetNextProgram(nullptr);
                auto rate = std::stoul(a.substr(std::string("actionSampleRate").length()));
                UserSettings->Set("sample_rate", static_cast<int>(rate));
                auto bp = _ui->_blueprint->GetBlueprint();
                if(bp)
                  {
                    bp->SetSamplesPerSecond(static_cast<unsigned int>(rate));
                    if(wasplaying)
                      ProgramPlayer->SetNextProgram(bp);
                  }
              }
            else if(a.starts_with("actionPlaybackDevice"))
              {
                auto device = std::stoi(a.substr(std::string("actionPlaybackDevice").length()));
                UserSettings->Set("playback_device", device);
                ProgramPlayer->SetAudioDevice(device);
              }
            else
              std::cerr << "Invalid action: " << a << std::endl;
          });

  if(UserSettings->GetInt("theme") == 1)
    ThemeDark();

  bool snapping = UserSettings->GetBool("snap_to_grid");
  _ui->_blueprint->SetSnapToGrid(snapping);
  _ui->actionSnapToGrid->setChecked(snapping);
  
  int recentcount = UserSettings->GetInt("recent_files");
  std::vector<std::string> recents;
  for(int i = recentcount - 1; i >= 0; i--)
    recents.push_back(UserSettings->GetString("recent_file_" + std::to_string(i)));
  ClearRecentFiles();
  for(auto s : recents)
    AddToRecentFiles(s);

  for(auto category : cats)
    {
      if(UserSettings->GetBool("nodecategory_" + category + "_grid"))
        ToggleCategoryLayout(category);
      if(UserSettings->GetBool("nodecategory_" + category + "_collapsed"))
        ToggleCategoryExpand(category);
    }

  statusBar()->showMessage(QString::fromStdString("Welcome to FMSEdit v" PACKAGE_VERSION "."), 5000);

  _ui->_blueprint->UpdateWindowTitle();
  UpdateToolbarButtonStates();

  if(UserSettings->GetInt("window_x") >= 0)
    {
      move(UserSettings->GetInt("window_x"), UserSettings->GetInt("window_y"));
      resize(UserSettings->GetInt("window_width"), UserSettings->GetInt("window_height"));
    }

  _ui->_scrollArea_Blueprint->ensureVisible(2048, 2048);

  if(file_to_open)
    _ui->_blueprint->Load(std::string(file_to_open));
}


WidgetMainWindow::~WidgetMainWindow()
{
  delete _ui;
}


void WidgetMainWindow::ThemeDefault()
{
  UserSettings->Set("theme", 0);

  QIcon icon;
  icon.addFile(QString::fromUtf8("images/ButtonSnapToGridOff.png"), QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(QString::fromUtf8("images/ButtonSnapToGrid.png"), QSize(), QIcon::Normal, QIcon::On);
  _ui->actionSnapToGrid->setIcon(icon);

  setPalette(_default_palette);
}


void WidgetMainWindow::ThemeDark()
{
  UserSettings->Set("theme", 1);
  
  QIcon icon;
  icon.addFile(QString::fromUtf8("images/ButtonSnapToGridOffDark.png"), QSize(), QIcon::Normal, QIcon::Off);
  icon.addFile(QString::fromUtf8("images/ButtonSnapToGridDark.png"), QSize(), QIcon::Normal, QIcon::On);
  _ui->actionSnapToGrid->setIcon(icon);
  
  QPalette palette;
  QBrush brush(QColor(246, 245, 244, 255));
  brush.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
  QBrush brush1(QColor(46, 46, 46, 255));
  brush1.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Button, brush1);
  QBrush brush2(QColor(69, 69, 69, 255));
  brush2.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Light, brush2);
  QBrush brush3(QColor(57, 57, 57, 255));
  brush3.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
  QBrush brush4(QColor(23, 23, 23, 255));
  brush4.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
  QBrush brush5(QColor(31, 31, 31, 255));
  brush5.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
  palette.setBrush(QPalette::Active, QPalette::Text, brush);
  QBrush brush6(QColor(255, 255, 255, 255));
  brush6.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::BrightText, brush6);
  QBrush brush7(QColor(222, 221, 218, 255));
  brush7.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::ButtonText, brush7);
  QBrush brush8(QColor(28, 28, 28, 255));
  brush8.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Base, brush8);
  palette.setBrush(QPalette::Active, QPalette::Window, brush1);
  QBrush brush9(QColor(0, 0, 0, 255));
  brush9.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Shadow, brush9);
  QBrush brush10(QColor(48, 140, 198, 255));
  brush10.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::Highlight, brush10);
  palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush6);
  palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
  QBrush brush11(QColor(255, 255, 220, 255));
  brush11.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush11);
  palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush9);
  QBrush brush12(QColor(246, 245, 244, 128));
  brush12.setStyle(Qt::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush12);
#endif
  palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
  palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
  palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
  palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
  palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
  palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
  palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
  palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
  palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush7);
  palette.setBrush(QPalette::Inactive, QPalette::Base, brush8);
  palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
  palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush9);
  palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush10);
  palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush6);
  palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
  palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush11);
  palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush9);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush12);
#endif
  QBrush brush13(QColor(118, 118, 118, 255));
  brush13.setStyle(Qt::SolidPattern);
  palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush13);
  palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
  palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
  palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
  palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
  palette.setBrush(QPalette::Disabled, QPalette::Text, brush13);
  palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush13);
  palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
  palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush9);
  palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush13);
  palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
  palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush11);
  palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush9);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
  palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush12);
#endif
  setPalette(palette);
}


void WidgetMainWindow::BuildAndPlay()
{
  auto b = _ui->_blueprint->Build();
  if(!b)
    std::cerr << "Error while building the blueprint: something went wrong" << std::endl;

  ProgramPlayer->SetNextProgram(b);
  UpdateToolbarButtonStates();
}


void WidgetMainWindow::AddToRecentFiles(const std::string & filename)
{
  if(filename.empty())
    return;

  bool existed = false;
  for(unsigned int i = 0; i < _recent_files.size(); i++)
    if(_recent_files[i] == filename)
      {
        existed = true;
        if(i > 0)
          {
            for(auto j = i; j > 0; j--)
              _recent_files[j] = _recent_files[j - 1];
            _recent_files[0] = filename;
          }
      }
  if(!existed)
    {
      _recent_files.resize(_recent_files.size() + 1);
      if(_recent_files.size() > 1)
        for(auto i = _recent_files.size() - 1; i > 0; i--)
          _recent_files[i] = _recent_files[i - 1];
      _recent_files[0] = filename;
    }

  for(auto a : _recent_files_actions)
    _ui->menuRecentFiles->removeAction(a);
  _recent_files_actions.clear();

  for(auto r : _recent_files)
    {
      auto raction = new QAction(this);
      raction->setObjectName(QString::fromStdString("actionMenuRecent:" + r));
      raction->setText(QString::fromStdString(r));
      _ui->menuRecentFiles->addAction(raction);
      _recent_files_actions.push_back(raction);
    }
}


void WidgetMainWindow::ClearRecentFiles()
{
  _recent_files.clear();
  for(auto a : _recent_files_actions)
    _ui->menuRecentFiles->removeAction(a);
  _recent_files_actions.clear();
}


void WidgetMainWindow::ToggleCategoryLayout(const std::string & category)
{
  auto root = _ui->_addnodes->findChild<QWidget *>(QString::fromStdString("_category_" + category), Qt::FindChildrenRecursively);
  assert(root);
  
  bool gridlayout = !root->property("grid_layout").toBool();
  root->setProperty("grid_layout", gridlayout);
  UserSettings->Set("nodecategory_" + category + "_grid", gridlayout);

  QLayout * newlayout = nullptr;
  std::string icon_filename;
  if(gridlayout)
    {
      icon_filename = "images/ButtonLayoutGrid.png";
      auto l = new QGridLayout();
      int x = 0;
      int y = 0;
      for(auto b : root->children())
        {
          auto button = dynamic_cast<QPushButton *>(b);
          if(button)
            {
              l->addWidget(button, y, x, 1, 1);
              button->setText("");
              x++;
              if(x == 3)
                {
                  x = 0;
                  y++;
                }
            }
        }
      newlayout = l;
    }
  else
    {
      icon_filename = "images/ButtonLayoutVertical.png";
      auto l = new QVBoxLayout();
      auto RestoreLabel = [](QPushButton * button)
      {
        button->setText(button->property("button_label").toString());
      };
      for(auto b : root->children())
        {
          auto button = dynamic_cast<QPushButton *>(b);
          if(button)
            {
              l->addWidget(button);
              RestoreLabel(button);
            }
        }
      newlayout = l;
    }
  newlayout->setSpacing(2);
  newlayout->setContentsMargins(0, 0, 0, 0);

  QIcon icon; 
  icon.addFile(QString::fromStdString(icon_filename), QSize(), QIcon::Normal, QIcon::Off);
  auto layoutbutton = _ui->_addnodes->findChild<QPushButton *>(QString::fromStdString("_category_layout_" + category), Qt::FindChildrenRecursively);
  assert(layoutbutton);
  layoutbutton->setIcon(icon);
  
  delete root->layout();
  root->setLayout(newlayout);
}


void WidgetMainWindow::ToggleCategoryExpand(const std::string & category)
{
  auto root = _ui->_addnodes->findChild<QWidget *>(QString::fromStdString("_category_" + category), Qt::FindChildrenRecursively);
  assert(root);

  bool collapsed = !root->property("category_collapsed").toBool();
  root->setProperty("category_collapsed", collapsed);
  UserSettings->Set("nodecategory_" + category + "_collapsed", collapsed);

  std::string icon_filename;
  if(collapsed)
    icon_filename = "images/ButtonCollapsed.png";
  else
    icon_filename = "images/ButtonExpanded.png";
  QIcon icon;
  icon.addFile(QString::fromStdString(icon_filename), QSize(), QIcon::Normal, QIcon::Off);
  auto button = _ui->_addnodes->findChild<QPushButton *>(QString::fromStdString("_category_expand_" + category), Qt::FindChildrenRecursively);
  assert(button);
  button->setIcon(icon);

  root->setVisible(!collapsed);
}


void WidgetMainWindow::HelpAbout()
{
  auto dialog = new WidgetHelpAbout(this);
  dialog->show();
}


bool WidgetMainWindow::AskUserConfirmation()
{
  QMessageBox mb;

  mb.setText("The blueprint has been modified.");
  mb.setInformativeText("Do you want to save your changes?");
  mb.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  mb.setDefaultButton(QMessageBox::Save);
  auto answer = mb.exec();

  bool rv = false;
  switch(answer)
    {
    case QMessageBox::Save:
      if(_ui->_blueprint->GetFilename().empty())
        _ui->_blueprint->Save();
      else
        _ui->_blueprint->Save(_ui->_blueprint->GetFilename());
      rv = true; // todo: Only return true if the save actually completed (user did not cancel it) successfully (file io did not fail).
      break;
    case QMessageBox::Discard:
      rv = true;
      break;
    }
  return rv;
}


void WidgetMainWindow::closeEvent(QCloseEvent * event)
{
  SaveWindowSettings();
  if(!_ui->_blueprint->IsDirty() || AskUserConfirmation())
    event->accept();
  else
    event->ignore();
}


void WidgetMainWindow::SaveWindowSettings()
{
  for(unsigned int i = 0; i < _recent_files.size(); i++)
    UserSettings->Set("recent_file_" + std::to_string(i), _recent_files[i]);
  UserSettings->Set("recent_files", static_cast<int>(_recent_files.size()));

  UserSettings->Set("window_x", pos().x());
  UserSettings->Set("window_y", pos().y());
  UserSettings->Set("window_width", size().width());
  UserSettings->Set("window_height", size().height());
}


void WidgetMainWindow::UpdateToolbarButtonStates()
{
  auto bp = _ui->_blueprint;
  _ui->actionMenuSave->setEnabled(bp->IsDirty());
  _ui->actionUndo->setEnabled(bp->CanUndo());
  _ui->actionRedo->setEnabled(bp->CanRedo());
  _ui->actionMenuPlay->setEnabled(bp->CanRun());
  _ui->actionDelete->setEnabled(bp->GetSelectedNodesCount() > 0);
}


QWidget * WidgetMainWindow::GetCommentBorderWidget()
{
  return _ui->_blueprint_comment_border;
}

