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

#ifndef GITHUB_H
#define GITHUB_H

#include "Account.h"
#include <QJsonDocument>

class GitHub : public Account {
  Q_OBJECT

public:
  GitHub(const QString &username);

  Kind kind() const override;
  QString name() const override;
  QString host() const override;
  void connect(const QString &password = QString()) override;

  void requestForkParents(Repository *repo) override;
  virtual void createPullRequest(Repository *repo, const QString &ownerRepo,
                                 const QString &title, const QString &body,
                                 const QString &head, const QString &base,
                                 bool canModify) override;

  void requestComments(Repository *repo, const QString &oid) override;

  void authorize() override;
  bool isAuthorizeSupported() override;

  static QString defaultUrl();

private:
  using Callback = std::function<void(const QJsonObject &)>;

  void graphql(const QString &query, const Callback &callback);

  void rest(const QUrl &url, const QJsonDocument &doc = QJsonDocument(),
            const Callback &callback = Callback());

  QString mState;
};

#endif
