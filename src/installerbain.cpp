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

#include <iinstallationmanager.h>
#include <moddatachecker.h>
#include <iplugingame.h>

#include <QDir>
#include <QtPlugin>
#include <QMessageBox>

#include <log.h>

#include "installerbain.h"
#include "baincomplexinstallerdialog.h"


using namespace MOBase;


InstallerBAIN::InstallerBAIN()
  : m_MOInfo(nullptr)
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
  return VersionInfo(1, 1, 0, VersionInfo::RELEASE_FINAL);
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

void InstallerBAIN::onInstallationStart(QString const& archive, bool reinstallation, IModInterface* currentMod)
{
  // We reset some field and fetch the previously installed options:
  m_InstallerUsed = false;
  m_PreviousOptions.clear();
  m_SelectedOptions.clear();

  if (currentMod) {
    auto settings = currentMod->pluginSettings(name());
    for (auto& [name, value] : settings) {
      if (name.startsWith("option")) {
        m_PreviousOptions.append(value.toString());
      }
    }
  }
}
void InstallerBAIN::onInstallationEnd(EInstallResult result, IModInterface* newMod)
{
  if (result == EInstallResult::RESULT_SUCCESS && m_InstallerUsed) {
    newMod->clearPluginSettings(name());
    for (auto i = 0; i < m_SelectedOptions.size(); ++i) {
      newMod->setPluginSetting(name(), QString("option%1").arg(i), m_SelectedOptions[i]);
    }
  }
}


bool InstallerBAIN::isArchiveSupported(std::shared_ptr<const IFileTree> tree) const
{
  ModDataChecker* checker = m_MOInfo->managedGame()->feature<ModDataChecker>();

  if (!checker) {
    return false;
  }

  int numValidDirs = 0;
  int numInvalidDirs = 0;

  // each directory would have to serve as a top-level directory
  for (auto entry: *tree) {

    if (!entry->isDir()) {
      continue;
    }

    QString dirName = entry->name().toLower();

    // ignore fomod in case of combined fomod/bain packages.
    // dirs starting with -- are supposed to be ignored
    if (FileNameComparator::compare(dirName, "fomod") == 0 ||
      FileNameComparator::compare(dirName, "omod conversion data") == 0 ||
        dirName.startsWith("--")) {
      continue;
    }

    if (checker->dataLooksValid(entry->astree()) == ModDataChecker::CheckReturn::VALID) {
      ++numValidDirs;
    } else {
      ++numInvalidDirs;
    }
  }

  if (numValidDirs < 2) {
    // a complex bain package contains at least 2 directories to choose from
    return false;
  } else if (numInvalidDirs == 0) {
    return true;
  } else {
    return QMessageBox::question(parentWidget(), tr("May be BAIN installer"),
                                 tr("This installer looks like it may contain a BAIN installer but I'm not sure. "
                                    "Install as BAIN installer?"),
                                 QMessageBox::Yes | QMessageBox::No)
            == QMessageBox::Yes;
  }

  return true;
}

IPluginInstaller::EInstallResult InstallerBAIN::install(GuessedValue<QString> &modName, std::shared_ptr<IFileTree> &tree,
                                                        QString&, int&)
{
  auto entry = tree->find("package.txt", FileTreeEntry::FILE);

  QString packageTXT;
  if (entry != nullptr) {
    packageTXT = manager()->extractFile(entry);
  }

  BainComplexInstallerDialog dialog(tree, modName, m_PreviousOptions, packageTXT, parentWidget());

  int res = dialog.exec();

  if (res == QDialog::Accepted) {
    modName.update(dialog.getName(), GUESS_USER);

    // Update the tree:
    m_SelectedOptions = dialog.updateTree(tree);
    m_InstallerUsed = true;

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


#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
Q_EXPORT_PLUGIN2(installerBAIN, InstallerBAIN)
#endif
