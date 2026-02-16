#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlTableModel>
#include <QTableView>
#include <QWidget>

class QTimer;

class HomeScreen final : public QWidget {
  Q_OBJECT

public:
  explicit HomeScreen(QWidget *parent = nullptr);

  void setOrdersModel(QSqlTableModel *model);

signals:
  void createOrderRequested();
  void deleteOrderRequested(long long orderId);
  void detailsRequested(long long orderId);
  void editOrderRequested(long long orderId);

private:
  QPushButton *createOrderBtn;

  QLineEdit *searchEdit;
  QComboBox *statusCombo;

  QTableView *table;
  QSqlTableModel *model = nullptr;

  QTimer *searchDebounce;

  void applyFilter();
  void handleOpenContextMenu(const QPoint &pos);
  void handleDeleteOrder();
  void handleEditOrder();
  static QString escapeSqlString(QString s);
};
