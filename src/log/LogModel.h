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

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractItemModel>
#include <QIcon>

class LogEntry;
class QStyle;

class LogModel : public QAbstractItemModel {
public:
  enum Role { EntryRole = Qt::UserRole };

  LogModel(LogEntry *root, QStyle *style, QObject *parent = nullptr);

  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  QModelIndex index(LogEntry *entry) const;
  LogEntry *entry(const QModelIndex &index) const;

private:
  LogEntry *mRoot;

  QIcon mHintIcon;
  QIcon mWarningIcon;
  QIcon mErrorIcon;
};

#endif
