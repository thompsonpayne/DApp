#pragma once

#include <QDate>
#include <QString>

struct OrderDraft {
  QString customer;
  QString product;
  int quantity = 0;
  QString status;
  QDate orderDate;
};
