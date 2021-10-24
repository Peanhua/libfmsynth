/*
  libfmsynth
  Copyright (C) 2021  Steve Joni Yrjänä <joniyrjana@gmail.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Complete license can be found in the LICENSE file.
*/

#include "WidgetNodeFileOutput.hh"
#include "NodeFileOutput.hh"
#include <filesystem>
#include "QtIncludeBegin.hh"
#include "UiNode.hh"
#include "UiNodeFileOutput.hh"
#include <QFileDialog>
#include "QtIncludeEnd.hh"


WidgetNodeFileOutput::WidgetNodeFileOutput(QWidget * parent)
  : WidgetNode(parent, new NodeFileOutput, true, false),
    _node_file_output(dynamic_cast<NodeFileOutput *>(GetNode())),
    _ui_node_file_output(new Ui::NodeFileOutput)
{
  SetNodeType("FileOutput");
  _ui_node->_title->setIcon(QIcon::fromTheme("media-floppy"));
  _ui_node->_title->setText("File");
  SetConnectorsRanges();
  _ui_node_file_output->setupUi(_ui_node->_content);
  adjustSize();

  connect(_ui_node_file_output->_select_filename, &QPushButton::clicked,
          [this]([[maybe_unused]] bool checked)
          {
            auto filename = QFileDialog::getSaveFileName(this, "Save file", QString(), QString("Audio files (*.wav)")).toStdString();
            if(filename.length() == 0)
              return;
            auto curdir = std::filesystem::current_path().string();
            if(filename.starts_with(curdir))
              filename = filename.substr(curdir.length() + 1); // +1 for the last path separator, which hopefully is one character long
            if(!filename.ends_with(".wav"))
              filename += ".wav";
            _ui_node_file_output->_filename->setText(QString::fromStdString(filename));
          });

  ListenWidgetChanges({ _ui_node_file_output->_filename });
}


void WidgetNodeFileOutput::NodeToWidget()
{
  WidgetNode::NodeToWidget();
  _ui_node_file_output->_filename->setText(QString::fromStdString(_node_file_output->GetFilename()));
}


void WidgetNodeFileOutput::WidgetToNode()
{
  WidgetNode::WidgetToNode();
  _node_file_output->SetFilename(_ui_node_file_output->_filename->text().toStdString());
}
