#include "order_form_dialog.h"

#include <QFormLayout>
#include <QVBoxLayout>

OrderFormDialog::OrderFormDialog(QWidget *parent) : QDialog(parent) {
  initUI("Create Order");
}

OrderFormDialog::OrderFormDialog(const OrderRow &existing, QWidget *parent)
    : QDialog(parent) {
  initUI(QString("Edit order #%1").arg(existing.id));
  customerEdit.setText(existing.customer);
  productEdit.setText(existing.product);
  quantitySpin.setValue(existing.quantity);

  if (statusCombo.findText(existing.status) >= 0) {
    statusCombo.setCurrentText(existing.status);
  }
  if (existing.orderDate.isValid()) {
    dateEdit.setDate(existing.orderDate);
  }
}

void OrderFormDialog::initUI(const QString &windowTitle) {
  setWindowTitle(windowTitle);

  customerEdit.setPlaceholderText("e.g. Stark Industries");
  productEdit.setPlaceholderText("e.g. MarkII");

  quantitySpin.setRange(1, 1000000);
  quantitySpin.setValue(1);

  statusCombo.addItems(
      {"pending", "processing", "shipped", "delivered", "cancelled"});

  dateEdit.setCalendarPopup(true);
  dateEdit.setDate(QDate::currentDate());

  errorLabel.setStyleSheet("color: #b00020;");
  errorLabel.setWordWrap(true);
  errorLabel.hide();

  auto *form = new QFormLayout();
  form->addRow("Customer", &customerEdit);
  form->addRow("Product", &productEdit);
  form->addRow("Quantity", &quantitySpin);
  form->addRow("Status", &statusCombo);
  form->addRow("Order date", &dateEdit);

  buttons.setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  // auto *title = new QLabel("Create an order", this);

  auto *root = new QVBoxLayout(this);
  // root->addWidget(title);
  root->addLayout(form);
  root->addWidget(&errorLabel);
  root->addWidget(&buttons);

  connect(&buttons, &QDialogButtonBox::accepted, this, [this] { onSubmit(); });
  connect(&buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void OrderFormDialog::onSubmit() {
  errorLabel.hide();
  errorLabel.clear();

  const auto customer = customerEdit.text().trimmed();
  const auto product = productEdit.text().trimmed();

  if (customer.isEmpty()) {
    return showError("Customer is required.");
  }
  if (product.isEmpty()) {
    return showError("Product is required.");
  }

  draft.customer = customer;
  draft.product = product;
  draft.quantity = quantitySpin.value();
  draft.status = statusCombo.currentText();
  draft.orderDate = dateEdit.date();

  accept();
}

void OrderFormDialog::showError(const QString &msg) {
  errorLabel.setText(msg);
  errorLabel.show();
}
