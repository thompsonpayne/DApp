#include "main_window.h"
#include "QSplitter"
#include "home_screen.h"
#include "order_form_dialog.h"
#include <QDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <optional>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("LogisticsApp");

  auto *root = new QSplitter(Qt::Horizontal, this);

  // Sidebar (left)
  auto *sidebar = new QWidget(root);
  sidebar->setObjectName("sidebar");
  sidebar->setMinimumWidth(180);
  auto *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setContentsMargins(12, 12, 12, 12);
  sidebarLayout->setSpacing(8);

  auto *ordersBtn = new QPushButton("Order", sidebar);
  auto *backBtn = new QPushButton("Back", sidebar);
  backBtn->setEnabled(false);

  sidebarLayout->addWidget(ordersBtn);
  sidebarLayout->addStretch(1);
  sidebarLayout->addWidget(backBtn);

  // Main content (right)
  stack = new QStackedWidget(root);
  home = new HomeScreen(stack);
  detail = new DetailScreen(stack);
  ordersModel = nullptr;

  stack->addWidget(home);
  stack->addWidget(detail);
  stack->setCurrentWidget(home);

  root->addWidget(sidebar);
  root->addWidget(stack);
  root->setStretchFactor(0, 0);
  root->setStretchFactor(1, 1);
  root->setSizes({220, 580});
  setCentralWidget(root);

  // Sidebar actions
  connect(ordersBtn, &QPushButton::clicked, this, [this] {
    history.clear();
    stack->setCurrentWidget(home);
  });

  connect(backBtn, &QPushButton::clicked, this, [this] { back(); });

  // Sync Back button enable state
  connect(stack, &QStackedWidget::currentChanged, this,
          [this, backBtn] { backBtn->setEnabled(!history.empty()); });

  // Home button events
  connect(home, &HomeScreen::createOrderRequested, this,
          [this] { handleCreateOrder(); });

  connect(home, &HomeScreen::deleteOrderRequested, this,
          [this](long long orderId) { handleDeleteOrder(orderId); });

  connect(home, &HomeScreen::detailsRequested, this,
          [this](long long orderId) { handleOpenDetails(orderId); });
  connect(detail, &DetailScreen::backRequested, this, [this] { back(); });

  connect(home, &HomeScreen::editOrderRequested, this,
          [this](long long orderId) { handleEditOrder(orderId); });

  if (!db.open() || !db.migrate()) {
    QMessageBox::critical(this, "Database error", db.lastError());
    setEnabled(false);
    return;
  }

  ordersModel = new QSqlTableModel(this);
  ordersModel->setTable("orders");
  ordersModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ordersModel->setSort(0, Qt::DescendingOrder); // id desc
  ordersModel->select();

  ordersModel->setHeaderData(1, Qt::Horizontal, "Customer");
  ordersModel->setHeaderData(2, Qt::Horizontal, "Product");
  ordersModel->setHeaderData(3, Qt::Horizontal, "Qty");
  ordersModel->setHeaderData(4, Qt::Horizontal, "Status");
  ordersModel->setHeaderData(5, Qt::Horizontal, "Date");

  home->setOrdersModel(ordersModel);
  resize(800, 600);
}

void MainWindow::goTo(QWidget *next) {
  if (!next || next == stack->currentWidget()) {
    return;
  }
  history.push_back(stack->currentWidget());
  stack->setCurrentWidget(next);
}

void MainWindow::back() {
  if (history.empty()) {
    return;
  }
  auto *prev = history.back();
  history.pop_back();
  stack->setCurrentWidget(prev);
}

void MainWindow::handleCreateOrder() {
  OrderFormDialog dlg(this);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  const auto draft = dlg.value();
  const auto insertedId = db.insertOrder(draft);
  if (!insertedId) {
    QMessageBox::critical(this, "Database error", db.lastError());
    return;
  }

  if (ordersModel) {
    ordersModel->select();
  }
}

void MainWindow::handleOpenDetails(long long orderId) {
  std::optional<OrderRow> order = db.getOrder(orderId);
  if (!order.has_value()) {
    return;
  }

  detail->setOrder(*order);
  goTo(detail);
}

void MainWindow::handleDeleteOrder(long long orderId) {
  if (!db.deleteOrder(orderId)) {
    QMessageBox::critical(this, "Database error", db.lastError());
    return;
  }

  if (ordersModel) {
    ordersModel->select();
  }
}

void MainWindow::handleEditOrder(long long orderId) {
  std::optional<OrderRow> order = db.getOrder(orderId);
  if (!order.has_value()) {
    if (!db.lastError().isEmpty()) {
      QMessageBox::critical(this, "Database error", db.lastError());
    } else {
      QMessageBox::information(this, "Not found",
                               QString("Order #%1 not found.").arg(orderId));
    }
    return;
  }

  OrderFormDialog dlg(*order, this);
  if (dlg.exec() != QDialog::Accepted) {
    return;
  }

  if (!db.updateOrder(orderId, dlg.value())) {
    QMessageBox::critical(this, "Database error", db.lastError());
    return;
  }

  if (ordersModel) {
    ordersModel->select();
  }
}
