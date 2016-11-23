#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>
#include <atomic>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <chrono>
#include <memory>

#include "parser/SchemaParser.hpp"
#include "parser/Schema.hpp"
#include "db.h"
#include "Types.hpp"
#include "query.h"

using namespace std;
using namespace std::chrono;

void newOrder(Database* db, int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t* supware, int32_t itemid[], int32_t qty[], Timestamp datetime) {
    auto w_tax = db->warehouse.row(make_tuple(w_id)).w_tax;
    auto c_discount = db->customer.row(make_tuple(w_id, d_id, c_id)).c_discount;
    auto district = db->district.row(make_tuple(w_id, d_id));
    auto o_id = district.d_next_o_id;
    auto d_tax = district.d_tax;
    district.d_next_o_id += 1;
    db->district.update(district);

    int32_t all_local = 1;
    for (int i = 0; i < items; i++) {
        if (w_id != supware[i]) {
            all_local = 0;
        }
    }


    db->order.insert({o_id, d_id, w_id, c_id, datetime, 0, items, all_local});
    db->neworder.insert({o_id, d_id, w_id});

    //forsequence (index between 0 and items-1) {
    for (int index = 0; index < items; ++index) {
        //select i_price from item where i_id=itemid[index];
        const auto i_price = db->item.row(make_tuple(itemid[index])).i_price;

        //select s_quantity,s_remote_cnt,s_order_cnt,case d_id ... as s_dist from stock where s_w_id=supware[index] and s_i_id=itemid[index];
        auto stock = db->stock.row(make_tuple(supware[index], itemid[index]));
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
        db->stock.update(stock);

        stock = db->stock.row(make_tuple(w_id, itemid[index]));
        if (supware[index] != w_id) {
            //update stock set s_remote_cnt=s_remote_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
            stock.s_remote_cnt = s_remote_cnt + 1;
        } else {
            //update stock set s_order_cnt=s_order_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
            stock.s_order_cnt = s_order_cnt + 1;
        }
        db->stock.update(stock);

        //var numeric(6,2) ol_amount=qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
        auto numericOne = Numeric<4, 4>(1.0);
        auto tmpTax = ((numericOne + w_tax + d_tax) * (numericOne - c_discount)).castS<6>();
        auto tmpPrice = (Numeric<5, 2>(qty[index]) * i_price).castS<6>();
        Numeric<6, 2> ol_amount = (tmpTax.castM2<6>().castM2<6>() * tmpPrice).castM2<6>().castM2<6>().castM2<6>();

        db->orderline.insert({o_id, d_id, w_id, index + 1, itemid[index], supware[index], 0, qty[index], ol_amount, s_dist});
    }
}

void delivery(Database* db, Integer w_id, Integer o_carrier_id, Timestamp datetime) {

    //Iterate over all districts
    for (int d_id = 1; d_id < 10; d_id++) {
        const auto o_id_itr = db->neworder.pkTree.lower_bound(std::make_tuple(w_id, d_id, INT32_MIN));
        if (o_id_itr == db->neworder.pkTree.end() || std::get<0>(o_id_itr->first) != w_id || std::get<1>(o_id_itr->first) != d_id) {
            continue; //If nothing is found, skip this district
        }
        const auto order = db->neworder.row(o_id_itr->first);
        const auto o_id = order.no_o_id;
        db->neworder.remove(db->neworder.pk[o_id_itr->first]);

        auto o = db->order.row(std::make_tuple(w_id, d_id, o_id));
        const auto o_ol_cnt = o.o_ol_cnt;
        const auto o_c_id = o.o_c_id;
        o.o_carrier_id = o_carrier_id;
        db->order.update(o);

        Numeric<6, 2> ol_total = 0;
        for (int ol_number = 1; Numeric<2, 0>(ol_number) < o_ol_cnt; ol_number += 1) {
            auto ol = db->orderline.row(std::make_tuple(w_id, d_id, o_id, ol_number));
            const auto ol_amount = ol.ol_amount;
            ol_total = ol_total + ol_amount;
            ol.ol_delivery_d = datetime;
            db->orderline.update(ol);
        }

        auto customer = db->customer.row(std::make_tuple(w_id, d_id, o_c_id));
        customer.c_balance = customer.c_balance + ol_total.castS<12>();
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

void newOrderRandom(Database* db) {
    int warehouses = 5;
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

    newOrder(db, w_id, d_id, c_id, ol_cnt, supware, itemid, qty, Timestamp(0));
}

void deliveryRandom(Database* db) {
    delivery(db, urand(1, 5), urand(1, 10), Timestamp(0));
}

Numeric<12, 4> statSum(Database* db) {
    // c_id, c_d_id, c_w_id
    std::unordered_map<std::tuple<Integer, Integer, Integer>, size_t> customers{};
    auto& cust = db->customer.table;
    for (size_t i = 0; i < cust.size(); i++) {
        if (cust[i].c_last.value[0] == 'B') { // c_last like 'B%'
            customers[std::make_tuple(cust[i].c_id, cust[i].c_d_id, cust[i].c_w_id)] = i;
        }
    }

    // o_id, o_d_id, o_w_id /
    std::unordered_map<std::tuple<Integer, Integer, Integer>, std::tuple<size_t, size_t>> orders{};
    auto& ord = db->order.table;
    for (size_t i = 0; i < ord.size(); i++) {
        auto item = customers.find(std::make_tuple(ord[i].o_c_id, ord[i].o_d_id, ord[i].o_w_id)); // o_w_id = c_w_id and o_d_id = c_d_id and o_c_id = c_id
        if (item != customers.end()) { //If found, add the index to the new hashmap
            orders[std::make_tuple(ord[i].o_id, ord[i].o_d_id, ord[i].o_w_id)] = std::make_tuple(i,
                                                                                                 item->second); // Save the customer index in the table as well, so that we don't have to find it later
        }
    }

    Numeric<12, 4> sum = 0;
    auto& ordLine = db->orderline.table;
    for (size_t i = 0; i < ordLine.size(); i++) {
        auto order = orders.find(std::make_tuple(ordLine[i].ol_o_id, ordLine[i].ol_d_id, ordLine[i].ol_w_id)); //o_w_id = ol_w_id and o_d_id = ol_d_id and o_id = ol_o_id
        if (order != orders.end()) { //If found, sum it up
            //sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
            sum += ordLine[i].ol_quantity.castS<12>().castP2() * ordLine[i].ol_amount.castS<12>() -
                   cust[std::get<1>(order->second)].c_balance * ord[std::get<0>(order->second)].o_ol_cnt.castS<12>().castP2();
        }
    }

    return sum;
}

static void runQuery(Database* db, int iterations) {
    int totalSeconds = 0;
    Numeric<12, 4> result;
    for (int i = 0; i < iterations; i++) {
        auto begin = high_resolution_clock::now();
        result = statSum(db);
        auto end = high_resolution_clock::now();
        totalSeconds += duration_cast<milliseconds>(end - begin).count();
    }
    cout << "Query result (" << iterations << "x): " << result << " took on average " << totalSeconds / iterations << "ms" << endl;
}

atomic<bool> childRunning;

static void SIGCHLD_handler(int /*sig*/) {
    int status;
    pid_t childPid = wait(&status);
    // now the child with process id childPid is dead
    childRunning = false;
}

int main(int argc, char** argv) {
    //Init our fork logic
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = SIGCHLD_handler;
    sigaction(SIGCHLD, &sa, NULL);

    //Start up the database
    Database* db = new Database();

    //Fixed number of iterations
#ifdef ENABLE_DEBUG_MACRO
    long iterations = 10000;
#else
    long iterations = 1000000;
#endif

    cout << "TPC-C Testrun" << endl;
    cout << "--------------------------" << endl;

    //Load data into "db"
    try {
        cout << "Loading initial db data: " << endl;
        auto begin = high_resolution_clock::now();
        db->import("../tbl/");
        cout << "done loading in " << duration_cast<seconds>(high_resolution_clock::now() - begin).count() << " seconds." << endl;


        query(db);
        delete db;
        return 0;

        cout << endl << "Starting simulation with " << iterations << " iterations ..." << endl << endl;
        runQuery(db, 10);

        begin = high_resolution_clock::now();
        int deliveries = 0, newOrders = 0;

        for (int i = 0; i < iterations; i++) {
            auto begin = high_resolution_clock::now();
            pid_t pid = ~0;

            //Start the child running the query if it finished
            if (!childRunning) {
                childRunning = true;
                pid = fork();
            }

            if (pid) { // parent
                if (urand(1, 100) <= 10) {
                    deliveryRandom(db);
                    deliveries++;
                } else {
                    newOrderRandom(db);
                    newOrders++;
                }
            } else { // forked child
                auto end = high_resolution_clock::now();
                cout << "Child fork took " << duration_cast<milliseconds>(end - begin).count() << "ms" << endl;
                runQuery(db, 1);
                return 0; // child is finished
            }
            if (i % (iterations / 10) == 0) {
                cout << ((double) i / (double) iterations * 100.0) << "% done" << endl;
            }
        }
        auto end = high_resolution_clock::now();
        // wait for child finishing
        while(childRunning);

        cout << "done. " << "Took: " << duration_cast<seconds>(end - begin).count() << " seconds." << endl;
        cout << "Transactions per second: " << iterations / duration_cast<seconds>(end - begin).count() << endl;
        cout << "New Orders: " << newOrders << " / Deliveries: " << deliveries << " / Ratio " << ((double) deliveries / (double) newOrders) * 100 << "%" << endl;
        cout << "Counts: " << db->order.size() << " orders | " << db->neworder.size() << " newOrders | " << db->orderline.size() << " orderlines " << endl;

    } catch (std::exception const& exc) {
        std::cerr << "Exception caught " << exc.what() << "\n";
    } catch (char const* str) {
        std::cerr << str;
    }

    delete db;
    return 0;
}
