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

#ifndef INSTALLERBAIN_H
#define INSTALLERBAIN_H


#include <iplugininstallersimple.h>


class InstallerBAIN : public MOBase::IPluginInstallerSimple
{
  Q_OBJECT
  Q_INTERFACES(MOBase::IPlugin MOBase::IPluginInstaller MOBase::IPluginInstallerSimple)
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
  Q_PLUGIN_METADATA(IID "org.tannin.InstallerBAIN" FILE "installerbain.json")
#endif

public:

  InstallerBAIN();

  virtual bool init(MOBase::IOrganizer *moInfo);
  virtual QString name() const;
  virtual QString author() const;
  virtual QString description() const;
  virtual MOBase::VersionInfo version() const;
  virtual bool isActive() const;
  virtual QList<MOBase::PluginSetting> settings() const;

  virtual unsigned int priority() const;
  virtual bool isManualInstaller() const;

  virtual bool isArchiveSupported(std::shared_ptr<const MOBase::IFileTree> tree) const;
  virtual EInstallResult install(MOBase::GuessedValue<QString> &modName, std::shared_ptr<MOBase::IFileTree> &tree,
                                 QString &version, int &modID);

private:

  bool isValidTopLayer(const MOBase::IFileTree *tree) const;

  /**
   * @brief Test if the specified directory qualifies as a top-level directory. A top-level directory
   * is one that contains data used by the game and goes directly below the "data" directory.
   * The most common examples are "textures" and "meshes". On top of these, this function will 
   * also accept directories that are not used by the game itself but by the BAIN installer ("ini tweaks" 
   * and docs)
   *
   * @param dirName The directory name to test.
   *
   * @return true if the specified directory is a top-level directory
   **/
  static bool isTopLevelDirectoryBain(const QString& dirName);

  /**
   * Test if the specified file qualifies as a top-level file. A top-level file is one
   * that will be used by the game if it's directly inside the "data" directory.
   * These are ".esp", ".esm", ".esl" and ".bsa".
   *
   * @param suffix Extension of the file to test.

   * @return bool
   **/
  static bool isTopLevelSuffix(const QString& suffix);

private:

  const MOBase::IOrganizer *m_MOInfo;

};

#endif // INSTALLERBAIN_H
