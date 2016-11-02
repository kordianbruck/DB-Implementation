
#ifndef DATABASE_H
#define DATABASE_H

#include <cstdint>
#include "table.h"

class Database {
public:
    void import(const std::string &path);

    Table<Warehouse> warehouses;
    Table<District> districts;
    Table<Customer> customers;
    Table<History> histories;
    Table<NewOrder> newOrders;
    Table<Order> orders;
    Table<OrderLine> orderLines;
    Table<Item> items;
    Table<Stock> stocks;
private:

};

#endif // DATABASE_H
