#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>

#include "Types.hpp"
#include "Tables.hpp"
using namespace std;


static vector<std::string> lineChunks;
static void split(string str, string sep) {
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    lineChunks.clear();
    current=strtok(cstr, sep.c_str());
    while(current!=NULL) {
        lineChunks.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
}

template <typename T>
static void loadFile(string file, vector<T>* db) {
    ifstream myfile(file);
    if (!myfile.is_open()) {
        return;
    }

    string line;
    while ( getline (myfile,line) ) {
        split(line,"|");
        db->push_back(T::parse(lineChunks));
    }
}

//Init our vectors holding all the data
static int baseSize = 1000;
static vector<Warehouse> warehouses(baseSize);
static unordered_map<Integer, u_int32_t> warehousesPrimaryKey();

static vector<District> districts(baseSize);
static unordered_map<tuple<Integer, Integer>, u_int32_t> districtsPrimaryKey();

static vector<Customer> customers(baseSize);
static unordered_map<tuple<Integer, Integer, Integer>, u_int32_t> customersPrimaryKey();

static vector<History> histories(baseSize);

static vector<NewOrder> newOrders(baseSize);
static unordered_map<tuple<Integer, Integer, Integer>, u_int32_t> newOrdersPrimaryKey();

static vector<Order> orders(baseSize);
static unordered_map<tuple<Integer, Integer, Integer>, u_int32_t> ordersPrimaryKey();

static vector<OrderLine> orderLines(baseSize);
static unordered_map<tuple<Integer, Integer, Integer, Integer>, u_int32_t> orderLinesPrimaryKey();

static vector<Item> items(baseSize);
static unordered_map<Integer, u_int32_t> itemsPrimaryKey();

static vector<Stock> stocks(baseSize);
static unordered_map<tuple<Integer, Integer>, u_int32_t> stocksPrimaryKey();

int main(int argc, char **argv) {
    cout << "TPC-C Testrun" << endl;
    cout << "--------------------------" << endl;

    //Load data into "db"
    try {
        clock_t begin = clock();
        cout << "Loading: " << endl;
        loadFile("../tbl/tpcc_warehouse.tbl", &warehouses);
        cout << "\tWarehouses: " << warehouses.size() << endl;

        loadFile("../tbl/tpcc_district.tbl", &districts);
        cout << "\tDistricts: " << districts.size() << endl;

        loadFile("../tbl/tpcc_customer.tbl", &customers);
        cout << "\tCustomers: " << customers.size() << endl;

        loadFile("../tbl/tpcc_history.tbl", &histories);
        cout << "\tHistories: " << histories.size() << endl;

        loadFile("../tbl/tpcc_neworder.tbl", &newOrders);
        cout << "\tNewOrders: " << newOrders.size() << endl;

        loadFile("../tbl/tpcc_order.tbl", &orders);
        cout << "\tOrders: " << orders.size() << endl;

        loadFile("../tbl/tpcc_orderline.tbl", &orderLines);
        cout << "\tOrderLines: " << orderLines.size() << endl;

        loadFile("../tbl/tpcc_item.tbl", &items);
        cout << "\tItems: " << items.size() << endl;

        loadFile("../tbl/tpcc_stock.tbl", &stocks);
        cout << "\tStock: " << stocks.size() << endl;

        clock_t end = clock();
        cout << "done. Took:" << (double(end - begin) / CLOCKS_PER_SEC) << " seconds." << endl;
    } catch (std::exception const &exc) {
        std::cerr << "Exception caught " << exc.what() << "\n";
    } catch (char const* str) {
        std::cerr << str;
    }
    
    return 0;
}



void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t* supware, int32_t* itemid, int32_t* qty, Timestamp datetime) {
    /*select w_tax from warehouse w where w.w_id=w_id;
    select c_discount from customer c where c_w_id=w_id and c_d_id=d_id and c.c_id=c_id;
    select d_next_o_id as o_id,d_tax from district d where d_w_id=w_id and d.d_id=d_id;
    update district set d_next_o_id=o_id+1 where d_w_id=w_id and district.d_id=d_id;

    int32_t all_local = 1;
    forsequence (index between 0 and items-1) {
        if (w_id != supware[index])
            all_local=0;
    }

    insert into "order" values (o_id,d_id,w_id,c_id,datetime,0,items,all_local);
    insert into neworder values (o_id,d_id,w_id);

    forsequence (index between 0 and items-1) {
        select i_price from item where i_id=itemid[index];
    select s_quantity,s_remote_cnt,s_order_cnt,case d_id when 1 then s_dist_01 when 2 then s_dist_02 when 3 then s_dist_03 when 4 then s_dist_04 when 5 then s_dist_05 when 6 then s_dist_06 when 7 then s_dist_07 when 8 then s_dist_08 when 9 then s_dist_09 when 10 then s_dist_10 end as s_dist from stock where s_w_id=supware[index] and s_i_id=itemid[index];

        if (s_quantity>qty[index]) {
            update stock set s_quantity=s_quantity-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
        } else {
            update stock set s_quantity=s_quantity+91-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
        }

        if (supware[index]<>w_id) {
            update stock set s_remote_cnt=s_remote_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
        } else {
            update stock set s_order_cnt=s_order_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
        }

        var numeric(6,2) ol_amount=qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
        insert into orderline values (o_id,d_id,w_id,index+1,itemid[index],supware[index],0,qty[index],ol_amount,s_dist);
    }
    commit;
    */


};


int32_t urand(int32_t min,int32_t max) {
    return (random()%(max-min+1))+min;
}

int32_t urandexcept(int32_t min,int32_t max,int32_t v) {
    if (max<=min)
        return min;
    int32_t r=(random()%(max-min))+min;
    if (r>=v)
        return r+1;
    else
        return r;
}

int32_t nurand(int32_t A,int32_t x,int32_t y) {
    return ((((random()%A)|(random()%(y-x+1)+x))+42)%(y-x+1))+x;
}

void newOrderRandom() {
    int warehouses = 5;
    Timestamp now(0);
    int32_t w_id=urand(1,warehouses);
    int32_t d_id=urand(1,10);
    int32_t c_id=nurand(1023,1,3000);
    int32_t ol_cnt=urand(5,15);

    int32_t supware[15];
    int32_t itemid[15];
    int32_t qty[15];
    for (int32_t i=0; i<ol_cnt; i++) {
        if (urand(1,100)>1)
            supware[i]=w_id;
        else
            supware[i]=urandexcept(1,warehouses,w_id);
        itemid[i]=nurand(8191,1,100000);
        qty[i]=urand(1,10);
    }

    newOrder(w_id,d_id,c_id,ol_cnt,supware,itemid,qty,now);
}

