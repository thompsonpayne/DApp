#include "home_screen.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>

HomeScreen::HomeScreen(QWidget *parent) : QWidget(parent) {
  createOrderBtn = new QPushButton("Create Order", this);

  searchEdit = new QLineEdit(this);
  statusCombo = new QComboBox(this);
  statusCombo->addItems(
      {"All", "pending", "processing", "shipped", "delivered", "cancelled"});
  searchEdit->setPlaceholderText("Search orders...");

  table = new QTableView(this);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setSortingEnabled(true);
  table->setContextMenuPolicy(Qt::CustomContextMenu);
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  auto *deleteAction = new QAction(table);
  deleteAction->setShortcut(QKeySequence::Delete);
  deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  table->addAction(deleteAction);

  auto *editAction = new QAction(table);
  editAction->setShortcut(QKeySequence(Qt::Key_F2));
  editAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  table->addAction(editAction);

  searchDebounce = new QTimer(this);
  searchDebounce->setSingleShot(true);
  searchDebounce->setInterval(250);

  auto *header = table->horizontalHeader();
  header->setStretchLastSection(true);
  header->setSectionResizeMode(QHeaderView::Stretch);

  auto *filters = new QHBoxLayout();
  filters->addWidget(searchEdit);
  filters->addWidget(statusCombo);

  auto *layout = new QVBoxLayout(this);
  layout->addWidget(createOrderBtn);
  layout->addLayout(filters);
  layout->addWidget(table);

  connect(searchEdit, &QLineEdit::textChanged, this,
          [this] { searchDebounce->start(); });
  connect(searchDebounce, &QTimer::timeout, this, [this] { applyFilter(); });
  connect(statusCombo, &QComboBox::currentTextChanged, this,
          [this] { applyFilter(); });

  connect(header, &QHeaderView::sortIndicatorChanged, this,
          [this](int column, Qt::SortOrder order) {
            if (!model) {
              return;
            }
            model->setSort(column, order);
            model->select();
          });

  connect(createOrderBtn, &QPushButton::clicked, this,
          [this] { emit createOrderRequested(); });

  connect(table, &QTableView::customContextMenuRequested, this,
          [this](const QPoint &pos) { handleOpenContextMenu(pos); });
  connect(deleteAction, &QAction::triggered, this,
          [this] { handleDeleteOrder(); });
  connect(editAction, &QAction::triggered, this, [this] { handleEditOrder(); });

  connect(table, &QTableView::doubleClicked, this,
          [this] { handleEditOrder(); });
}

void HomeScreen::applyFilter() {
  if (!model) {
    return;
  }

  const auto term = searchEdit->text().trimmed();
  const auto status = statusCombo->currentText();

  QStringList parts;

  if (!term.isEmpty()) {
    const auto t = escapeSqlString(term);
    parts << QString("(customer LIKE '%%1%' OR product LIKE '%%1%' OR status "
                     "LIKE '%%1%')")
                 .arg(t);
  }

  if (status != "All") {
    parts << QString("status = '%1'").arg(escapeSqlString(status));
  }

  model->setFilter(parts.join(" AND "));
  model->select();
}

void HomeScreen::setOrdersModel(QSqlTableModel *m) {
  model = m;
  table->setModel(model);
  table->setColumnHidden(0, true);
  applyFilter();
}

QString HomeScreen::escapeSqlString(QString s) { return s.replace("'", "''"); }

void HomeScreen::handleOpenContextMenu(const QPoint &pos) {
  if (!model) {
    return;
  }

  const QModelIndex idx = table->indexAt(pos);
  if (!idx.isValid()) {
    return;
  }
  table->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect |
                                           QItemSelectionModel::Rows);

  const int row = idx.row();
  const long long orderId = model->data(model->index(row, 0)).toLongLong();

  QMenu menu(this);
  QAction *viewDetailAction = menu.addAction("View Order Details");
  QAction *editOrderAction = menu.addAction("Edit Order");
  QAction *deleteAction = menu.addAction("Delete Order");
  QAction *chosen = menu.exec(table->viewport()->mapToGlobal(pos));

  if (chosen == deleteAction) {
    handleDeleteOrder();
  }
  if (chosen == viewDetailAction) {
    emit detailsRequested(orderId);
  }
  if (chosen == editOrderAction) {
    handleEditOrder();
  }
}

void HomeScreen::handleDeleteOrder() {
  if (!model) {
    return;
  }

  const auto idx = table->selectionModel()->currentIndex();
  if (!idx.isValid()) {
    QMessageBox::information(this, "Select order",
                             "Select an order to delete.");
    return;
  }

  const auto row = idx.row();
  if (row < 0) {
    QMessageBox::information(this, "Select order",
                             "Select an order to delete.");
    return;
  }

  const auto orderId = model->data(model->index(row, 0)).toLongLong();
  const auto res = QMessageBox::question(
      this, "Delete order", QString("Delete order #%1?").arg(orderId),
      QMessageBox::Yes | QMessageBox::No);
  if (res != QMessageBox::Yes) {
    return;
  }

  emit deleteOrderRequested(orderId);
}

void HomeScreen::handleEditOrder() {
  if (!model) {
    return;
  }

  const auto idx = table->selectionModel()->currentIndex();
  if (!idx.isValid()) {
    QMessageBox::information(this, "Select order", "Select an order to edit");
    return;
  }

  const auto row = idx.row();
  const auto orderId = model->data(model->index(row, 0)).toLongLong();
  emit editOrderRequested(orderId);
}
