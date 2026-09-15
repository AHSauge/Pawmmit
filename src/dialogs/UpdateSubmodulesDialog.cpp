//
//          Copyright (c) 2016, Scientific Toolworks, Inc.
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Jason Haslam
//

#include "UpdateSubmodulesDialog.h"
#include "git/Repository.h"
#include "git/Submodule.h"
#include "ui_UpdateSubmodulesDialog.h"
#include <QAbstractTableModel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>

namespace {

class Model : public QAbstractTableModel {
public:
  Model(const git::Repository &repo, QObject *parent = nullptr)
      : QAbstractTableModel(parent) {
    for (const git::Submodule &submodule : repo.submodules())
      mSubmodules.append({true, submodule});
  }

  QList<git::Submodule> enabledSubmodules() const {
    QList<git::Submodule> submodules;
    for (const Entry &entry : mSubmodules) {
      if (entry.enabled)
        submodules.append(entry.submodule);
    }

    return submodules;
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    return 2;
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    return mSubmodules.size();
  }

  Qt::ItemFlags flags(const QModelIndex &index) const override {
    return QAbstractTableModel::flags(index) |
           ((index.column() == 0) ? Qt::ItemIsUserCheckable : Qt::NoItemFlags);
  }

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override {
    const Entry &entry = mSubmodules.at(index.row());
    switch (role) {
      case Qt::CheckStateRole:
        if (index.column() == 0)
          return entry.enabled ? Qt::Checked : Qt::Unchecked;
        break;

      case Qt::DisplayRole:
        if (index.column() == 1)
          return entry.submodule.name();
        break;
    }

    return QVariant();
  }

  bool setData(const QModelIndex &index, const QVariant &value,
               int role) override {
    if (index.column() != 0)
      return false;

    mSubmodules[index.row()].enabled = value.toBool();
    emit dataChanged(index, index);
    return true;
  }

private:
  struct Entry {
    bool enabled;
    git::Submodule submodule;
  };

  QList<Entry> mSubmodules;
};

} // namespace

UpdateSubmodulesDialog::UpdateSubmodulesDialog(const git::Repository &repo,
                                               QWidget *parent)
    : QDialog(parent), ui(new Ui::UpdateSubmodulesDialog) {
  setAttribute(Qt::WA_DeleteOnClose);

  ui->setupUi(this);

  Model *model = new Model(repo, this);
  ui->mTable->setModel(model);

  ui->mTable->verticalHeader()->hide();
  ui->mTable->horizontalHeader()->hide();
  ui->mTable->horizontalHeader()->setStretchLastSection(false);
  ui->mTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);

  QPushButton *update =
      ui->mButtons->addButton(tr("Update"), QDialogButtonBox::AcceptRole);
  update->setEnabled(!model->enabledSubmodules().isEmpty());
  connect(ui->mButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(ui->mButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  connect(model, &Model::dataChanged, [model, update] {
    update->setEnabled(!model->enabledSubmodules().isEmpty());
  });
}

UpdateSubmodulesDialog::~UpdateSubmodulesDialog() = default;

QList<git::Submodule> UpdateSubmodulesDialog::submodules() const {
  return static_cast<Model *>(ui->mTable->model())->enabledSubmodules();
}

bool UpdateSubmodulesDialog::recursive() const {
  return ui->mRecursive->isChecked();
}

bool UpdateSubmodulesDialog::init() const { return ui->mInit->isChecked(); }
