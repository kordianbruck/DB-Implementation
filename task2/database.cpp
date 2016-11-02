
#include "database.h"

using namespace std;

void Database::import(const string& path) {
        this->warehouses.loadTableFromFile(path + "tpcc_warehouse.tbl");
        cout << "\tWarehouses: " << this->warehouses.size() << endl;

        this->districts.loadTableFromFile(path + "tpcc_district.tbl");
        cout << "\tDistricts: " << districts.size() << endl;

        this->customers.loadTableFromFile(path + "tpcc_customer.tbl");
        cout << "\tCustomers: " << customers.size() << endl;

        this->histories.loadTableFromFile(path + "tpcc_history.tbl");
        cout << "\tHistories: " << histories.size() << endl;

        this->newOrders.loadTableFromFile(path + "tpcc_neworder.tbl");
        cout << "\tNewOrders: " << newOrders.size() << endl;

        this->orders.loadTableFromFile(path + "tpcc_order.tbl");
        cout << "\tOrders: " << orders.size() << endl;

        this->orderLines.loadTableFromFile(path + "tpcc_orderline.tbl");
        cout << "\tOrderLines: " << orderLines.size() << endl;

        this->items.loadTableFromFile(path + "tpcc_item.tbl");
        cout << "\tItems: " << items.size() << endl;

        this->stocks.loadTableFromFile(path + "tpcc_stock.tbl");
        cout << "\tStock: " << stocks.size() << endl;
}
