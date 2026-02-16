#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "database.h"

class DetailScreen final : public QWidget {
  Q_OBJECT

public:
  explicit DetailScreen(QWidget *parent = nullptr);

  void setOrder(const OrderRow &order);

signals:
  void backRequested();

private:
  QLabel *customerValue;
  QLabel *productValue;
  QLabel *quantityValue;
  QLabel *statusValue;
  QLabel *dateValue;
};
