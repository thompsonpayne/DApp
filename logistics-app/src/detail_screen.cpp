#include "detail_screen.h"

#include <QFormLayout>
#include <QVBoxLayout>

DetailScreen::DetailScreen(QWidget *parent) : QWidget(parent) {
  auto *title = new QLabel("Order Details", this);
  title->setStyleSheet("font-weight: 600;");

  customerValue = new QLabel("-", this);
  productValue = new QLabel("-", this);
  quantityValue = new QLabel("-", this);
  statusValue = new QLabel("-", this);
  dateValue = new QLabel("-", this);

  auto *form = new QFormLayout();
  form->addRow("Customer", customerValue);
  form->addRow("Product", productValue);
  form->addRow("Quantity", quantityValue);
  form->addRow("Status", statusValue);
  form->addRow("Order date", dateValue);

  auto *backBtn = new QPushButton("Back", this);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(title);
  layout->addLayout(form);
  layout->addWidget(backBtn);

  connect(backBtn, &QPushButton::clicked, this,
          [this] { emit backRequested(); });
}

void DetailScreen::setOrder(const OrderRow &order) {
  customerValue->setText(order.customer);
  productValue->setText(order.product);
  quantityValue->setText(QString::number(order.quantity));
  statusValue->setText(order.status);
  dateValue->setText(order.orderDate.toString(Qt::ISODate));
}
