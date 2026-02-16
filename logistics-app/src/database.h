#pragma once

#include <QDate>
#include <QString>
#include <optional>
#include <vector>

#include "models.h"

struct OrderRow {
  long long id = 0;
  QString customer;
  QString product;
  int quantity = 0;
  QString status;
  QDate orderDate;
};

class Database final {
public:
  bool open();
  bool migrate();

  std::optional<long long> insertOrder(const OrderDraft &order);
  std::vector<OrderRow> listOrders();
  std::optional<OrderRow> getOrder(long long orderId);
  bool updateOrder(long long orderId, const OrderDraft &order);
  bool deleteOrder(long long orderId);

  QString lastError() const { return lastErr; }

private:
  QString lastErr;
  QString dbPath() const;
};
