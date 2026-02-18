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

struct UserRow {
  long long id;
  QString username;
  QString role;
};

class Database final {
public:
  bool open();
  bool migrate();

  // order
  std::optional<long long> insertOrder(const OrderDraft &order);
  std::vector<OrderRow> listOrders();
  std::optional<OrderRow> getOrder(long long orderId);
  bool updateOrder(long long orderId, const OrderDraft &order);
  bool deleteOrder(long long orderId);

  // user
  bool hasAnyUsers();
  std::optional<UserRow> createUser(const QString &username,
                                    const QString &password,
                                    const QString &role);
  std::optional<UserRow> verifyUser(const QString &username,
                                    const QString &password);

  // error
  QString lastError() const { return lastErr; }

private:
  QString lastErr;
  QString dbPath() const;
};
