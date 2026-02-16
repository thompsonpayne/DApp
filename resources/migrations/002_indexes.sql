CREATE INDEX IF NOT EXISTS idx_orders_status
ON orders(status);
CREATE INDEX IF NOT EXISTS idx_orders_order_date
ON orders(order_date);
CREATE INDEX IF NOT EXISTS idx_orders_customer
ON orders(customer);
CREATE INDEX IF NOT EXISTS idx_orders_product
ON orders(product);
