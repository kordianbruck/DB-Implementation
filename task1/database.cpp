
#include "database.h"

using namespace std;

void Database::import(const string& path) {        
        this->warehouses.loadTableFromFile(path + "tpcc_warehouse.tbl"s);
        cout << "\tWarehouses: " << this->warehouses.size() << endl;

        this->districts.loadTableFromFile(path + "tpcc_district.tbl"s);
        cout << "\tDistricts: " << districts.size() << endl;
        
        this->customers.loadTableFromFile(path + "tpcc_customer.tbl"s);
        cout << "\tCustomers: " << customers.size() << endl;
        
        this->histories.loadTableFromFile(path + "tpcc_history.tbl"s);
        cout << "\tHistories: " << histories.size() << endl;
        
        this->newOrders.loadTableFromFile(path + "tpcc_neworder.tbl"s);
        cout << "\tNewOrders: " << newOrders.size() << endl;
        
        this->orders.loadTableFromFile(path + "tpcc_order.tbl"s);
        cout << "\tOrders: " << orders.size() << endl;

        this->orderLines.loadTableFromFile(path + "tpcc_orderline.tbl"s);
        cout << "\tOrderLines: "s << orderLines.size() << endl;

        this->items.loadTableFromFile(path + "tpcc_item.tbl"s);
        cout << "\tItems: "s << items.size() << endl;

        this->stocks.loadTableFromFile(path + "tpcc_stock.tbl"s);
        cout << "\tStock: "s << stocks.size() << endl;
}
