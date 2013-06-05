/*
Copyright (C) 2012 Sebastian Herbord. All rights reserved.

This file is part of Mod Organizer.

Mod Organizer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Mod Organizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Mod Organizer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "installerbain.h"
#include "baincomplexinstallerdialog.h"
#include <installationtester.h>
#include <iinstallationmanager.h>
#include <QDir>
#include <QtPlugin>


using namespace MOBase;


InstallerBAIN::InstallerBAIN()
{
}


bool InstallerBAIN::init(IOrganizer *moInfo)
{
  m_MOInfo = moInfo;
  return true;
}

QString InstallerBAIN::name() const
{
  return "BAIN Installer";
}

QString InstallerBAIN::author() const
{
  return "Tannin";
}

QString InstallerBAIN::description() const
{
  return tr("Installer for BAIN archives (originally targeting Wrye Bash)");
}

VersionInfo InstallerBAIN::version() const
{
  return VersionInfo(1, 0, 0, VersionInfo::RELEASE_FINAL);
}

bool InstallerBAIN::isActive() const
{
  return true;
}

QList<PluginSetting> InstallerBAIN::settings() const
{
  return QList<PluginSetting>();
}

unsigned int InstallerBAIN::priority() const
{
  return 40;
}

bool InstallerBAIN::isManualInstaller() const
{
  return false;
}

bool InstallerBAIN::isValidTopLayer(const DirectoryTree::Node *node) const
{
  // see if there is at least one directory that makes sense on the top level
  for (DirectoryTree::const_node_iterator iter = node->nodesBegin(); iter != node->nodesEnd(); ++iter) {
    if (InstallationTester::isTopLevelDirectoryBain((*iter)->getData().name)) {
      qDebug("%s on the top level", (*iter)->getData().name.toUtf8().constData());
      return true;
    }
  }

  // see if there is a file that makes sense on the top level
  for (DirectoryTree::const_leaf_iterator iter = node->leafsBegin(); iter != node->leafsEnd(); ++iter) {
    if (InstallationTester::isTopLevelSuffix(iter->getName())) {
      return true;
    }
  }

  return false;
}

bool InstallerBAIN::isArchiveSupported(const DirectoryTree &tree) const
{
  int numDirs = tree.numNodes();
  // each directory would have to serve as a top-level directory
  for (DirectoryTree::const_node_iterator iter = tree.nodesBegin();
       iter != tree.nodesEnd(); ++iter) {
    const QString &dirName = (*iter)->getData().name;

    // ignore fomod in case of combined fomod/bain packages.
    // dirs starting with -- are supposed to be ignored
    if ((dirName.compare("fomod", Qt::CaseInsensitive) == 0) ||
        (dirName.startsWith("--"))) {
      --numDirs;
      continue;
    }

    if (!isValidTopLayer(*iter)) {
      return false;
    }
  }

  if (numDirs < 2) {
    // a complex bain package contains at least 2 directories to choose from
    return false;
  }

  return true;
}

IPluginInstaller::EInstallResult InstallerBAIN::install(GuessedValue<QString> &modName, DirectoryTree &tree,
                                                        QString&, int&)
{
  QString packageTXT = manager()->extractFile("package.txt");

  BainComplexInstallerDialog dialog(&tree, modName, packageTXT, parentWidget());

  int res = dialog.exec();
  QFile::remove(QDir::tempPath().append("/package.txt"));

  if (res == QDialog::Accepted) {
    modName.update(dialog.getName(), GUESS_USER);

    // create a new tree with the selected directories mapped to the
    // base directory. This is destructive on the original tree
    DirectoryTree *newTree = dialog.updateTree(&tree);
    tree = *newTree;
    delete newTree;
    return IPluginInstaller::RESULT_SUCCESS;
  } else {
    if (dialog.manualRequested()) {
      modName.update(dialog.getName(), GUESS_USER);
      return IPluginInstaller::RESULT_MANUALREQUESTED;
    } else {
      return IPluginInstaller::RESULT_CANCELED;
    }
  }
}

Q_EXPORT_PLUGIN2(installerBAIN, InstallerBAIN)
