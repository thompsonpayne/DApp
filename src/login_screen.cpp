#include "login_screen.h"
#include "database.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

LoginScreen::LoginScreen(Database *db_, QWidget *parent)
    : QWidget(parent), db(db_) {
  titleLabel = new QLabel("Admin sign in", this);
  titleLabel->setStyleSheet("font-size: 18px; font-weight: 600;");

  hintLabel = new QLabel(this);
  hintLabel->setWordWrap(true);

  errorLabel = new QLabel(this);
  errorLabel->setWordWrap(true);
  errorLabel->setStyleSheet("color: #B00020;");

  usernameEdit = new QLineEdit(this);
  usernameEdit->setPlaceholderText("Username");

  passwordEdit = new QLineEdit(this);
  passwordEdit->setPlaceholderText("Password");
  passwordEdit->setEchoMode(QLineEdit::Password);

  confirmEdit = new QLineEdit(this);
  confirmEdit->setPlaceholderText("Confirm password");
  confirmEdit->setEchoMode(QLineEdit::Password);

  showPasswordCheck = new QCheckBox("Show password", this);

  primaryBtn = new QPushButton("Login", this);
  // loginBtn->setDefault(true);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(48, 48, 48, 48);
  layout->setSpacing(10);
  layout->addWidget(titleLabel);
  layout->addWidget(hintLabel);
  layout->addSpacing(8);
  layout->addWidget(errorLabel);
  layout->addWidget(usernameEdit);
  layout->addWidget(passwordEdit);
  layout->addWidget(confirmEdit);
  layout->addWidget(showPasswordCheck);
  layout->addSpacing(8);
  layout->addWidget(primaryBtn);
  layout->addStretch(1);
  setMinimumSize(300, 200);

  connect(showPasswordCheck, &QCheckBox::toggled, this, [this](bool on) {
    const auto mode = on ? QLineEdit::Normal : QLineEdit::Password;
    passwordEdit->setEchoMode(mode);
    confirmEdit->setEchoMode(mode);
  });

  connect(primaryBtn, &QPushButton::clicked, this,
          [this] { attemptPrimary(); });

  connect(usernameEdit, &QLineEdit::returnPressed, this,
          [this] { passwordEdit->setFocus(); });

  connect(passwordEdit, &QLineEdit::returnPressed, this,
          [this] { attemptPrimary(); });
  connect(confirmEdit, &QLineEdit::returnPressed, this,
          [this] { attemptPrimary(); });

  refresh();
}

void LoginScreen::refresh() {
  setError({});

  const bool anyUsers = db && db->hasAnyUsers();
  setCreateAdminMode(!anyUsers);
}

void LoginScreen::setCreateAdminMode(bool enabled) {
  createAdminMode = enabled;

  if (createAdminMode) {
    titleLabel->setText("Create admin account");
    hintLabel->setText("No users exist yet. Create the first admin account.");
    confirmEdit->setVisible(true);
    primaryBtn->setText("Create admin");
  } else {
    titleLabel->setText("Admin sign in");
    hintLabel->setText("Sign in to access the admin system.");
    confirmEdit->setVisible(false);
    primaryBtn->setText("Sign in");
  }
}

void LoginScreen::setError(const QString &msg) {
  errorLabel->setText(msg);
  errorLabel->setVisible(!msg.isEmpty());
}

void LoginScreen::attemptPrimary() {
  setError({});

  if (!db) {
    setError("Database not available.");
    return;
  }

  const auto username = usernameEdit->text().trimmed();
  const auto password = passwordEdit->text();

  if (username.isEmpty()) {
    setError("Username is required.");
    return;
  }
  if (password.isEmpty()) {
    setError("Password is required.");
    return;
  }

  if (createAdminMode) {
    const auto confirm = confirmEdit->text();
    if (password.size() < 8) {
      setError("Password must be at least 8 characters.");
      return;
    }
    if (password != confirm) {
      setError("Passwords do not match.");
      return;
    }

    const auto created = db->createUser(username, password, "admin");
    if (!created) {
      setError(db->lastError().isEmpty() ? "Failed to create user."
                                         : db->lastError());
      return;
    }
    emit authenticated(created->id, created->username);
    return;
  } else {
    const auto user = db->verifyUser(username, password);
    if (!user) {
      setError(db->lastError().isEmpty() ? "Invalid username or password"
                                         : db->lastError());
      return;
    }
    emit authenticated(user->id, user->username);
  }
}
