//
//          Copyright (c) 2021, Gittyup
//
// This software is licensed under the GNU General Public License v3.0 or
// (at your option) any later version. The LICENSE.md file describes the
// conditions under which this software may be distributed.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Author: Kas (https://github.com/exactly-one-kas)
//

#include <QKeyEvent>
#include <QTreeView>

class HotkeysPanel : public QTreeView {
public:
  enum Column { Name, Kind, Description };

  HotkeysPanel(QWidget *parent = nullptr);

  virtual QSize sizeHint() const override;

protected:
  virtual bool edit(const QModelIndex &index,
                    QAbstractItemView::EditTrigger trigger,
                    QEvent *event) override;
  virtual void keyPressEvent(QKeyEvent *e) override;
};
