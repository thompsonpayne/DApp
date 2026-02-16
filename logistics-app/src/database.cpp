#include "database.h"
#include "models.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QTextStream>
#include <QVariant>
#include <optional>
#include <print>

namespace {
struct Migration {
  int version = 0;
  QString resourcePath;
};

QString readResource(const QString &path, QString &err) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    err = "Failed to open migration resource: " + path;
    return {};
  }

  QTextStream in(&file);
  return in.readAll();
}

QStringList splitSqlStatements(const QString &sql) {
  QStringList statements;
  QString buffer;
  bool inSingleQuote = false;

  for (int i = 0; i < sql.size(); ++i) {
    const auto c = sql.at(i);

    if (c == '\'' && (i == 0 || sql.at(i - 1) != '\\')) {
      inSingleQuote = !inSingleQuote;
    }

    if (!inSingleQuote && c == ';') {
      const auto stmt = buffer.trimmed();
      if (!stmt.isEmpty()) {
        statements.push_back(stmt);
      }
      buffer.clear();
      continue;
    }

    buffer.append(c);
  }

  const auto tail = buffer.trimmed();
  if (!tail.isEmpty()) {
    statements.push_back(tail);
  }

  return statements;
}

bool applyMigration(const Migration &m, QString &err) {
  const auto sql = readResource(m.resourcePath, err);
  if (sql.isEmpty()) {
    return false;
  }

  const auto statements = splitSqlStatements(sql);
  QSqlQuery q;
  for (const auto &stmt : statements) {
    if (!q.exec(stmt)) {
      err = q.lastError().text();
      return false;
    }
  }

  QSqlQuery setVersion;
  if (!setVersion.exec(QString("PRAGMA user_version = %1").arg(m.version))) {
    err = setVersion.lastError().text();
    return false;
  }

  return true;
}
} // namespace

QString Database::dbPath() const {
  const auto baseDir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return QDir(baseDir).filePath("logistics.sqlite");
}

bool Database::open() {
  lastErr.clear();

  if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
    lastErr = "QSQLITE driver not available. Available drivers: " +
              QSqlDatabase::drivers().join(", ");
    return false;
  }

  const auto path = dbPath();
  QDir().mkpath(QFileInfo(path).absolutePath());
  qDebug().noquote() << "SQLite DB path:" << path;

  auto db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(path);

  if (!db.open()) {
    lastErr = db.lastError().text();
    return false;
  }

  return true;
}

bool Database::migrate() {
  lastErr.clear();

  QSqlQuery q;
  if (!q.exec("PRAGMA user_version")) {
    lastErr = q.lastError().text();
    return false;
  }

  int currentVersion = 0;
  if (q.next()) {
    currentVersion = q.value(0).toInt();
  }

  const std::vector<Migration> migrations = {
      {1, ":/migrations/001_init.sql"},
      {2, ":/migrations/002_indexes.sql"},
  };

  auto db = QSqlDatabase::database();
  for (const auto &m : migrations) {
    if (m.version <= currentVersion) {
      continue;
    }

    if (!db.transaction()) {
      lastErr = db.lastError().text();
      return false;
    }

    QString err;
    if (!applyMigration(m, err)) {
      db.rollback();
      lastErr = err;
      return false;
    }

    if (!db.commit()) {
      lastErr = db.lastError().text();
      return false;
    }

    currentVersion = m.version;
  }

  return true;
}

std::optional<long long> Database::insertOrder(const OrderDraft &o) {
  lastErr.clear();

  QSqlQuery q;
  q.prepare(R"SQL(
    INSERT INTO orders (customer, product, quantity, status, order_date)
    VALUES (?, ?, ?, ?, ?)
  )SQL");
  q.addBindValue(o.customer);
  q.addBindValue(o.product);
  q.addBindValue(o.quantity);
  q.addBindValue(o.status);
  q.addBindValue(o.orderDate.toString(Qt::ISODate));

  if (!q.exec()) {
    lastErr = q.lastError().text();
    return std::nullopt;
  }

  return q.lastInsertId().toLongLong();
}

std::vector<OrderRow> Database::listOrders() {
  lastErr.clear();

  std::vector<OrderRow> out;

  QSqlQuery q;
  if (!q.exec(R"SQL(
    SELECT id, customer, product, quantity, status, order_date
    FROM orders
    ORDER BY id DESC
  )SQL")) {
    lastErr = q.lastError().text();
    return out;
  }

  while (q.next()) {
    OrderRow r;
    r.id = q.value(0).toLongLong();
    r.customer = q.value(1).toString();
    r.product = q.value(2).toString();
    r.quantity = q.value(3).toInt();
    r.status = q.value(4).toString();
    r.orderDate = QDate::fromString(q.value(5).toString(), Qt::ISODate);
    out.push_back(std::move(r));
  }

  return out;
}

std::optional<OrderRow> Database::getOrder(long long orderId) {
  lastErr.clear();
  std::println("Fetching order {}", orderId);

  QSqlQuery q;
  q.prepare(R"SQL(
            select id, customer, product, quantity, status, order_date
            from orders
            where id = ?
            limit 1
            )SQL");
  q.addBindValue(orderId);

  if (!q.exec()) {
    lastErr = q.lastError().text();
    return std::nullopt;
  }

  if (!q.next()) {
    return std::nullopt;
  }

  OrderRow r;
  r.id = q.value(0).toLongLong();
  r.customer = q.value(1).toString();
  r.product = q.value(2).toString();
  r.quantity = q.value(3).toInt();
  r.status = q.value(4).toString();
  r.orderDate = QDate::fromString(q.value(5).toString(), Qt::ISODate);
  return r;
}

bool Database::deleteOrder(long long orderId) {
  lastErr.clear();

  QSqlQuery q;
  q.prepare("delete from orders where id = ?");
  q.addBindValue(orderId);

  if (!q.exec()) {
    lastErr = q.lastError().text();
    return false;
  }

  if (q.numRowsAffected() == 0) {
    lastErr = "Order not found.";
    return false;
  }

  return true;
};

bool Database::updateOrder(long long orderId, const OrderDraft &o) {
  lastError().clear();

  QSqlQuery q;
  q.prepare(R"sql(
            update orders
            set customer = ?, product = ?, quantity = ?, status = ?, order_date = ?
            where id = ?
            )sql");
  q.addBindValue(o.customer);
  q.addBindValue(o.product);
  q.addBindValue(o.quantity);
  q.addBindValue(o.status);
  q.addBindValue(o.orderDate.toString(Qt::ISODate));
  q.addBindValue(orderId);

  if (!q.exec()) {
    lastErr = q.lastError().text();
    return false;
  }
  if (q.numRowsAffected() == 0) {
    lastErr = "Order not found.";
    return false;
  }
  return true;
}
