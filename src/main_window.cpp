#include "main_window.h"

#include <QDialog>
#include <QMessageBox>
#include <QSize>
#include <QSizePolicy>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QStyle>
#include <optional>
#include <vector>

#include "order_form_dialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("LogisticsApp");
  constexpr int kSidebarCollapsedWidth = 56;

  sidebarLastWidth = 220;
  rootSplitter = new QSplitter(Qt::Horizontal, this);

  // Sidebar (left)
  auto *sidebar = new QWidget(rootSplitter);
  sidebar->setObjectName("sidebar");
  sidebar->setMinimumWidth(kSidebarCollapsedWidth);
  auto *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setContentsMargins(12, 12, 12, 12);
  sidebarLayout->setSpacing(8);

  const QIcon collapseIcon = style()->standardIcon(QStyle::SP_ArrowLeft);
  const QIcon expandIcon = style()->standardIcon(QStyle::SP_ArrowRight);

  auto initMenuButton = [this](QToolButton *button, const QString &label,
                              QStyle::StandardPixmap standardIcon,
                              bool defaultEnabled) {
    button->setText(label);
    button->setIcon(style()->standardIcon(standardIcon));
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIconSize(QSize(20, 20));
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    button->setToolTip(label);
    button->setEnabled(defaultEnabled);
  };

  auto *toggleBtn = new QToolButton(sidebar);
  toggleBtn->setCheckable(true);
  toggleBtn->setChecked(true); // expanded
  toggleBtn->setAutoRaise(true);
  toggleBtn->setIconSize(QSize(20, 20));
  toggleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toggleBtn->setToolTip("Collapse sidebar");

  auto *ordersBtn = new QToolButton(sidebar);
  initMenuButton(ordersBtn, "Orders", QStyle::SP_FileDialogListView, true);
  auto *backBtn = new QToolButton(sidebar);
  initMenuButton(backBtn, "Back", QStyle::SP_ArrowBack, false);

  const std::vector<QToolButton *> menuButtons = {ordersBtn, backBtn};

  sidebarLayout->addWidget(toggleBtn);
  sidebarLayout->addWidget(ordersBtn);
  sidebarLayout->addStretch(1);
  sidebarLayout->addWidget(backBtn);

  // Main content (right)
  stack = new QStackedWidget(rootSplitter);
  home = new HomeScreen(stack);
  detail = new DetailScreen(stack);
  ordersModel = nullptr;

  stack->addWidget(home);
  stack->addWidget(detail);
  stack->setCurrentWidget(home);

  rootSplitter->addWidget(sidebar);
  rootSplitter->addWidget(stack);
  rootSplitter->setStretchFactor(0, 0);
  rootSplitter->setStretchFactor(1, 1);
  rootSplitter->setSizes({sidebarLastWidth, 580});
  rootSplitter->setHandleWidth(0);
  setCentralWidget(rootSplitter);

  auto applySidebarExpanded =
      [this, sidebar, toggleBtn, menuButtons, collapseIcon, expandIcon,
       kSidebarCollapsedWidth](bool expanded) {
        if (!rootSplitter) {
          return;
        }

        const auto sizes = rootSplitter->sizes();
        int total = sizes.value(0) + sizes.value(1);
        if (total <= 0) {
          total = rootSplitter->width();
        }

        if (expanded) {
          sidebar->setMaximumWidth(QWIDGETSIZE_MAX);

          toggleBtn->setIcon(collapseIcon);
          toggleBtn->setText("Collapse");
          toggleBtn->setToolTip("Collapse sidebar");
          toggleBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

          for (auto *btn : menuButtons) {
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
          }

          if (total <= 0) {
            return;
          }

          const int desired = sidebarLastWidth > kSidebarCollapsedWidth
                                  ? sidebarLastWidth
                                  : 220;
          constexpr int kMinContentWidth = 240;
          const int maxSidebar =
              qMax(kSidebarCollapsedWidth, total - kMinContentWidth);
          const int w = qBound(kSidebarCollapsedWidth, desired, maxSidebar);
          rootSplitter->setSizes({w, qMax(0, total - w)});
        } else {
          const int currentW = sizes.value(0);
          if (currentW > kSidebarCollapsedWidth) {
            sidebarLastWidth = currentW;
          }

          sidebar->setMaximumWidth(kSidebarCollapsedWidth);

          toggleBtn->setIcon(expandIcon);
          toggleBtn->setText("Expand");
          toggleBtn->setToolTip("Expand sidebar");
          toggleBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);

          for (auto *btn : menuButtons) {
            btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
          }

          if (total <= 0) {
            return;
          }

          rootSplitter->setSizes(
              {kSidebarCollapsedWidth,
               qMax(0, total - kSidebarCollapsedWidth)});
        }
      };

  connect(toggleBtn, &QToolButton::toggled, this,
          [applySidebarExpanded](bool expanded) { applySidebarExpanded(expanded); });

  // Ensure initial state styling is consistent after layout.
  QTimer::singleShot(0, this,
                     [applySidebarExpanded] { applySidebarExpanded(true); });

  // Sidebar actions
  connect(ordersBtn, &QToolButton::clicked, this, [this] {
    history.clear();
    stack->setCurrentWidget(home);
  });

  connect(backBtn, &QToolButton::clicked, this, [this] { back(); });

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
