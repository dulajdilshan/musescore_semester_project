//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id: palette.cpp 5576 2012-04-24 19:15:22Z wschweer $
//
//  Copyright (C) 2011 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#include "pluginManager.h"
#include "shortcutcapturedialog.h"
#include "musescore.h"
#include <QDesktopServices>
#include <QUrl>

#include <iostream>

namespace Ms {

//---------------------------------------------------------
//   PluginManager
//---------------------------------------------------------

PluginManager::PluginManager(QWidget* parent)
   : QDialog(parent)
      {
      setObjectName("PluginManager");
      setupUi(this);
      setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
      readSettings();

      //add system plugins to all plugins tab

      all->addItem(new QListWidgetItem("ABC_import"));
      all->addItem(new QListWidgetItem("Colornotes"));
      all->addItem(new QListWidgetItem("Createscore"));
      all->addItem(new QListWidgetItem("Notenames"));
      all->addItem(new QListWidgetItem("Panel"));
      all->addItem(new QListWidgetItem("Random"));
      all->addItem(new QListWidgetItem("Random2"));
      all->addItem(new QListWidgetItem("run"));
      all->addItem(new QListWidgetItem("Scorelist"));
      all->addItem(new QListWidgetItem("Walk"));
      }

//---------------------------------------------------------
//   init
//---------------------------------------------------------

void PluginManager::init()
      {
      //
      //plugin list item changes when reload the window
      //
      loadList(false);
      connect(pluginList, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(pluginLoadToggled(QListWidgetItem*)));
      connect(allPlugins, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(pluginLoadToggled(QListWidgetItem*)));
      connect(pluginList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         SLOT(pluginListItemChanged(QListWidgetItem*, QListWidgetItem*)));
      }

//---------------------------------------------------------
//   loadList - populate the listbox.
//---------------------------------------------------------

void PluginManager::loadList(bool forceRefresh)
      {
      QStringList saveLoaded; // If forcing a refresh, the load flags are lost. Keep a copy and reapply.
      int n = preferences.pluginList.size();
      if (forceRefresh && n > 0) {

          //refreash all the plugins in plugin manager
            for (int i = 0; i < n; i++) {
                  PluginDescription& d = preferences.pluginList[i];
                  if (d.load) {
                        saveLoaded.append(d.path);
                        mscore->unregisterPlugin(&d);  // This will force the menu to rebuild.
                        }
                  }
            }

      //load plugin path from preferences
      preferences.updatePluginList(forceRefresh);
      n = preferences.pluginList.size();
      pluginList->clear();
      allPlugins->clear();

      //load plugins for List widgets
      for (int i = 0; i < n; ++i) {
            PluginDescription& d = preferences.pluginList[i];
            Shortcut* s = &d.shortcut;
            localShortcuts[s->key()] = new Shortcut(*s);
            if (saveLoaded.contains(d.path)) d.load = true;

            //add all available plugins to installation list widget to install
            QListWidgetItem* item = new QListWidgetItem(QFileInfo(d.path).completeBaseName(),  pluginList);
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
            item->setCheckState(d.load ? Qt::Checked : Qt::Unchecked);
            item->setData(Qt::UserRole, i);

            //add all available plugins to all tab to display
            QListWidgetItem* item2 = new QListWidgetItem(QFileInfo(d.path).completeBaseName(),  allPlugins);
            item2->setFlags(item2->flags() | Qt::ItemIsEnabled);

            //add all newly installed plugins to installed tab to display
            if (QFileInfo(d.path).completeBaseName()!="Walk" && QFileInfo(d.path).completeBaseName()!="Colornotes" && QFileInfo(d.path).completeBaseName()!="Createscore"
                    && QFileInfo(d.path).completeBaseName()!="Random2" && QFileInfo(d.path).completeBaseName()!="run" && QFileInfo(d.path).completeBaseName()!="Scorelist"
                    && QFileInfo(d.path).completeBaseName()!="Notenames" && QFileInfo(d.path).completeBaseName()!="Panel" && QFileInfo(d.path).completeBaseName()!="Random"
                    && QFileInfo(d.path).completeBaseName()!="Walk"){
                QListWidgetItem* item3 = new QListWidgetItem(QFileInfo(d.path).completeBaseName(),  installedPlugins);
                item2->setFlags(item3->flags() | Qt::ItemIsEnabled);
            }
      }

      prefs = preferences;
      if (n) {
            pluginList->setCurrentRow(0);
            pluginListItemChanged(pluginList->item(0), 0);
            }
      }

//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void PluginManager::accept()
      {

    //register and unregister plugins

      int n = prefs.pluginList.size();
      for (int i = 0; i < n; ++i) {
            PluginDescription& d = prefs.pluginList[i];
            if (d.load)
                  mscore->registerPlugin(&d);
            else
                  mscore->unregisterPlugin(&d);
            }
      preferences = prefs;
      preferences.write();
      disconnect(pluginList, SIGNAL(itemChanged(QListWidgetItem*)));
      disconnect(pluginList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
      QDialog::accept();
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void PluginManager::closeEvent(QCloseEvent* ev)
      {
      emit closed(false);
      QWidget::closeEvent(ev);
      }

//---------------------------------------------------------
//   pluginListItemChanged
//---------------------------------------------------------

void PluginManager::pluginListItemChanged(QListWidgetItem* item, QListWidgetItem*)
      {
    //view plugin details : plugin name, plugin path, plugin version, plugin distription
      if (!item)
            return;
      int idx = item->data(Qt::UserRole).toInt();
      const PluginDescription& d = prefs.pluginList[idx];
      QFileInfo fi(d.path);
      pluginName->setText(fi.completeBaseName());
      pluginPath->setText(fi.absolutePath());
      pluginVersion->setText(d.version);
      pluginDescription->setText(d.description);


      }
void PluginManager::installed(QListWidgetItem *item){
    if(item->checkState() == Qt::Checked)
        item->setBackgroundColor(QColor("#ffffb2"));
}
//---------------------------------------------------------
//   pluginLoadToggled
//---------------------------------------------------------


void PluginManager::pluginLoadToggled(QListWidgetItem* item)
      {
    //check states of plugins:states:registered or unregistered

      int idx = item->data(Qt::UserRole).toInt();
      PluginDescription* d = &prefs.pluginList[idx];
      d->load = (item->checkState() == Qt::Checked);
      prefs.dirty = true;
      }


//---------------------------------------------------------
//   writeSettings
//---------------------------------------------------------

void PluginManager::writeSettings()
      {
      MuseScore::saveGeometry(this);
      }

//---------------------------------------------------------
//   readSettings
//---------------------------------------------------------

void PluginManager::readSettings()
      {
      MuseScore::restoreGeometry(this);
      }

}

void Ms::PluginManager::on_pushButton_2_clicked()
{
    // connect to online plugin store

    QString link = "https://musescoreonlinepluginstore.000webhostapp.com/src/Login.php";
    QDesktopServices::openUrl(QUrl(link));
}

