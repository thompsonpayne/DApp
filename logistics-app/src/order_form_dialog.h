#pragma once

#include <QComboBox>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include "database.h"
#include "models.h"

class OrderFormDialog final : public QDialog {
public:
  explicit OrderFormDialog(QWidget *parent = nullptr);
  OrderFormDialog(const OrderRow &existing, QWidget *parent = nullptr);

  OrderDraft value() const { return draft; }

private:
  QLineEdit customerEdit;
  QLineEdit productEdit;
  QSpinBox quantitySpin;
  QComboBox statusCombo;
  QDateEdit dateEdit;
  QLabel errorLabel;
  QDialogButtonBox buttons;
  OrderDraft draft;

  void onSubmit();
  void showError(const QString &msg);
  void initUI(const QString &windowTitle);
};
