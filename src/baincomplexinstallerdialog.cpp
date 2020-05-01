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

#include "baincomplexinstallerdialog.h"
#include "textviewer.h"
#include "ui_baincomplexinstallerdialog.h"

#include <QCompleter>


using namespace MOBase;


BainComplexInstallerDialog::BainComplexInstallerDialog(
  std::shared_ptr<MOBase::IFileTree> tree, const GuessedValue<QString> &modName, 
  const QString &packageTXT, QWidget *parent)
  : TutorableDialog("BainInstaller", parent), ui(new Ui::BainComplexInstallerDialog), m_Manual(false),
    m_PackageTXT(packageTXT)
{
  ui->setupUi(this);

  for (auto iter = modName.variants().begin(); iter != modName.variants().end(); ++iter) {
    ui->nameCombo->addItem(*iter);
  }

  ui->nameCombo->setCurrentIndex(ui->nameCombo->findText(modName));

  for (auto &entry: *tree) {
    const QString &dirName = entry->name().toLower();
    
    
    if (!entry->isDir() || dirName == "fomod" || dirName.startsWith("--")) {
      continue;
    }

    QListWidgetItem *item = new QListWidgetItem(ui->optionsList);
    item->setText(dirName);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(item->text().mid(0, 2) == "00" ? Qt::Checked : Qt::Unchecked);

    ui->optionsList->addItem(item);
  }

  ui->packageBtn->setEnabled(!packageTXT.isEmpty());
  ui->nameCombo->completer()->setCaseSensitivity(Qt::CaseSensitive);
}


BainComplexInstallerDialog::~BainComplexInstallerDialog()
{
  delete ui;
}


QString BainComplexInstallerDialog::getName() const
{
  return ui->nameCombo->currentText();
}

void BainComplexInstallerDialog::updateTree(std::shared_ptr<IFileTree> &tree)
{
  // Note (Holt59): The version with DirectoryTree was a bit convoluted
  // as the only things that need to be done is removed directory not selected,
  // and these directory can only be at the root of the tree.

  std::set<QString, FileNameComparator> selectedNames;
  for (int i = 0; i < ui->optionsList->count(); ++i) {
    QListWidgetItem* item = ui->optionsList->item(i);
    if (item->checkState() == Qt::Checked) {
      selectedNames.insert(item->text());
    }
  }

  // Start from an empty tree and insert everything (easier since
  // we cannot modify the tree while iterating):
  auto newTree = tree->createOrphanTree();
  for (auto& entry : *tree) {
    if (entry->isDir()) {
      if (selectedNames.count(entry->name()) > 0) {
        newTree->merge(entry->astree());
      }
    }
    else {
      newTree->insert(entry);
    }
  }

  tree = newTree;
}


void BainComplexInstallerDialog::on_okBtn_clicked()
{
  this->accept();
}


void BainComplexInstallerDialog::on_cancelBtn_clicked()
{
  this->reject();
}


void BainComplexInstallerDialog::on_manualBtn_clicked()
{
  m_Manual = true;
  this->reject();
}

void BainComplexInstallerDialog::on_packageBtn_clicked()
{
  TextViewer viewer(m_PackageTXT, this);
  viewer.setDescription("");
  viewer.addFile(m_PackageTXT, false);
  viewer.exec();
}
