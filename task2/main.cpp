#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>
#include <iostream>
#include <memory>

#include "parser/Parser.hpp"
#include "parser/Schema.hpp"
#include "database.h"

using namespace std;

void newOrder(Database *db, int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t *supware, int32_t itemid[], int32_t qty[], Timestamp datetime) {
    auto w_tax = db->warehouses.row(make_tuple(w_id)).w_tax;
    auto c_discount = db->customers.row(make_tuple(w_id, d_id, c_id)).c_discount;
    auto district = db->districts.row(make_tuple(w_id, d_id));
    auto o_id = district.d_next_o_id;
    auto d_tax = district.d_tax;
    district.d_next_o_id += 1;
    db->districts.update(district);

    int32_t all_local = 1;
    for (int i = 0; i < items; i++) {
        if (w_id != supware[i]) {
            all_local = 0;
        }
    }

    Order *o = new Order();
    o->o_id = o_id;
    o->o_d_id = d_id;
    o->o_w_id = w_id;
    o->o_c_id = c_id;
    o->o_entry_d = datetime;
    o->o_carrier_id = 0;
    o->o_ol_cnt = items;
    o->o_all_local = all_local;
    db->orders.insert(o);

    NewOrder *no = new NewOrder();
    no->no_o_id = o_id;
    no->no_d_id = d_id;
    no->no_w_id = w_id;
    db->newOrders.insert(no);

    //forsequence (index between 0 and items-1) {
    for (int index = 0; index < items; ++index) {
        //select i_price from item where i_id=itemid[index];
        auto i_price = db->items.row(make_tuple(itemid[index])).i_price;

        //select s_quantity,s_remote_cnt,s_order_cnt,case d_id ... as s_dist from stock where s_w_id=supware[index] and s_i_id=itemid[index];
        auto stock = db->stocks.row(make_tuple(supware[index], itemid[index]));
        auto s_quantity = stock.s_quantity;
        auto s_remote_cnt = stock.s_remote_cnt;
        auto s_order_cnt = stock.s_order_cnt;
        Char<24> s_dist;
        switch (d_id) {
            default:
            case 1: s_dist = stock.s_dist_01;
                break;
            case 2: s_dist = stock.s_dist_02;
                break;
            case 3: s_dist = stock.s_dist_03;
                break;
            case 4: s_dist = stock.s_dist_04;
                break;
            case 5: s_dist = stock.s_dist_05;
                break;
            case 6: s_dist = stock.s_dist_06;
                break;
            case 7: s_dist = stock.s_dist_07;
                break;
            case 8: s_dist = stock.s_dist_01;
                break;
            case 9: s_dist = stock.s_dist_09;
                break;
            case 10: s_dist = stock.s_dist_10;
                break;
        }

        if (s_quantity > qty[index]) {
            //update stock set s_quantity=s_quantity-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
            stock.s_quantity = s_quantity - qty[index];
        } else {
            //update stock set s_quantity=s_quantity+91-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
            stock.s_quantity = s_quantity + 91 - qty[index];
        }
        db->stocks.update(stock);

        auto stockSecond = db->stocks.row(make_tuple(w_id, itemid[index]));
        if (supware[index] != w_id) {
            //update stock set s_remote_cnt=s_remote_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
            stock.s_remote_cnt = s_remote_cnt + 1;
        } else {
            //update stock set s_order_cnt=s_order_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
            stock.s_order_cnt = s_order_cnt + 1;
        }
        db->stocks.update(stock);

        //var numeric(6,2) ol_amount=qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
        auto numericOne = Numeric<4, 4>(1.0);
        auto tmpTax = ((numericOne + w_tax + d_tax) * (numericOne - c_discount)).castS<6>();
        auto tmpPrice = (Numeric<5, 2>(qty[index]) * i_price).castS<6>();
        Numeric<6, 2> ol_amount = (tmpTax.castM2<6>().castM2<6>() * tmpPrice).castM2<6>().castM2<6>().castM2<6>();

        //insert into orderline values (o_id,d_id,w_id,index+1,itemid[index],supware[index],0,qty[index],ol_amount,s_dist);
        OrderLine *ol = new OrderLine();
        ol->ol_o_id = o_id;
        ol->ol_d_id = d_id;
        ol->ol_w_id = w_id;
        ol->ol_number = index + 1;
        ol->ol_i_id = itemid[index];
        ol->ol_supply_w_id = supware[index];
        ol->ol_delivery_d = 0;
        ol->ol_quantity = qty[index];
        ol->ol_amount = ol_amount;
        ol->ol_dist_info = s_dist;
        db->orderLines.insert(ol);
    }
}


int32_t urand(int32_t min, int32_t max) {
    return (random() % (max - min + 1)) + min;
}

int32_t urandexcept(int32_t min, int32_t max, int32_t v) {
    if (max <= min) {
        return min;
    }
    int32_t r = (random() % (max - min)) + min;
    if (r >= v) {
        return r + 1;
    } else {
        return r;
    }
}

int32_t nurand(int32_t A, int32_t x, int32_t y) {
    return ((((random() % A) | (random() % (y - x + 1) + x)) + 42) % (y - x + 1)) + x;
}

void newOrderRandom(Database *db) {
    int warehouses = 5;
    Timestamp now(0);
    int32_t w_id = urand(1, warehouses);
    int32_t d_id = urand(1, 10);
    int32_t c_id = nurand(1023, 1, 3000);
    int32_t ol_cnt = urand(5, 15);

    int32_t supware[15];
    int32_t itemid[15];
    int32_t qty[15];
    for (int32_t i = 0; i < ol_cnt; i++) {
        if (urand(1, 100) > 1) {
            supware[i] = w_id;
        } else {
            supware[i] = urandexcept(1, warehouses, w_id);
        }
        itemid[i] = nurand(8191, 1, 100000);
        qty[i] = urand(1, 10);
    }

    newOrder(db, w_id, d_id, c_id, ol_cnt, supware, itemid, qty, now);
}

int main(int argc, char **argv) {
    Database *db = new Database();

    cout << "TPC-C Testrun" << endl;
    cout << "--------------------------" << endl;

    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
        return -1;
    }

    Parser p(argv[1]);
    try {
        std::unique_ptr<Schema> schema = p.parse();
        std::cout << schema->toString() << std::endl;
    } catch (ParserError &e) {
        std::cerr << e.what() << " on line " << e.where() << std::endl;
    }

    return 0;

    //Load data into "db"
    try {
        cout << "Loading: " << endl;
        clock_t begin = clock();
        db->import("../../tbl/");
        cout << "done. Took:" << (double(clock() - begin) / CLOCKS_PER_SEC) << " seconds." << endl;

        cout << endl << "Starting inserts..." << endl;
        begin = clock();
        for (int i = 0; i < 100000; i++) {
            newOrderRandom(db);
        }
        auto end = clock();
        cout << "done. " << "Took: " << (double(end - begin) / CLOCKS_PER_SEC) << " seconds." << endl;
        cout << "Transactions per second: " << 1000000.0 / (double(end - begin) / CLOCKS_PER_SEC) << endl;
        cout << "Counts: " << db->orders.size() << " orders | " << db->newOrders.size() << " newOrders | " << db->orderLines.size() << " orderLines " << endl;

    } catch (std::exception const &exc) {
        std::cerr << "Exception caught " << exc.what() << "\n";
    } catch (char const *str) {
        std::cerr << str;
    }

    return 0;
}
