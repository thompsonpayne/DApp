#pragma once

#include "QLineEdit"
#include "QPushButton"
#include "QWidget"
#include "database.h"
#include <QLabel>
#include <QCheckBox>

class LoginScreen final : public QWidget {
  Q_OBJECT

public:
  explicit LoginScreen(Database *db, QWidget *parent = nullptr);
  void refresh();

signals:
  void authenticated(long long userId, const QString &username);

private:
  Database *db;

  QLabel *titleLabel;
  QLabel *hintLabel;
  QLabel *errorLabel;

  QLineEdit *usernameEdit;
  QLineEdit *passwordEdit;
  QLineEdit *confirmEdit;
  QCheckBox *showPasswordCheck;
  QPushButton *primaryBtn;

  bool createAdminMode = false;

  void setCreateAdminMode(bool enabled);
  void attemptPrimary();
  void setError(const QString &msg);
};
