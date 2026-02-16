#pragma once

#include <QMainWindow>
#include <QSqlTableModel>
#include <QStackedWidget>
#include <vector>

#include "database.h"
#include "detail_screen.h"
#include "home_screen.h"

class MainWindow final : public QMainWindow {
public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  Database db;
  QSqlTableModel *ordersModel;
  QStackedWidget *stack;
  HomeScreen *home;
  DetailScreen *detail;
  std::vector<QWidget *> history;

  void goTo(QWidget *next);
  void back();
  void handleCreateOrder();
  void handleDeleteOrder(long long orderId);
  void handleOpenDetails(long long orderId);
  void handleEditOrder(long long orderId);
};
