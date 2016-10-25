#ifndef H_Tables
#define H_Tables

#include "Types.hpp"

class Warehouse {
public:
    Integer w_id; // PK
    Varchar<10> w_name;
    Varchar<20> w_street_1;
    Varchar<20> w_street_2;
    Varchar<20> w_city;
    Char<2> w_state;
    Char<9> w_zip;
    Numeric<4,4> w_tax;
    Numeric<12,2> w_ytd;
    
    inline static Warehouse parse(std::vector<std::string> row) {
        Warehouse* ret = new Warehouse();
        ret->w_id.castString(row[0].c_str(), row[0].length());
        ret->w_name.castString(row[1].c_str(), row[1].length());
        ret->w_street_1.castString(row[2].c_str(), row[2].length());
        ret->w_street_2.castString(row[3].c_str(), row[3].length());
        ret->w_city.castString(row[4].c_str(), row[4].length());
        ret->w_state.castString(row[5].c_str(), row[5].length());
        ret->w_zip.castString(row[6].c_str(), row[6].length());
        ret->w_tax.castString(row[7].c_str(), row[7].length());
        ret->w_ytd.castString(row[8].c_str(), row[8].length());
        return *ret;
    }
};

class District {
public:
    Integer d_id;
    Integer d_w_id;
    Varchar<10> d_name;
    Varchar<20> d_street_1;
    Varchar<20> d_street_2;
    Varchar<20> d_city;
    Char<2> d_state;
    Char<9> d_zip;
    Numeric<4,4> d_tax;
    Numeric<12,2> d_ytd;
    Integer d_next_o_id;
    
    inline static District parse(std::vector<std::string> row) {
        District* ret = new District();
        ret->d_id.castString(row[0].c_str(), row[0].length());
        ret->d_w_id.castString(row[1].c_str(), row[1].length());
        ret->d_name.castString(row[2].c_str(), row[2].length());
        ret->d_street_1.castString(row[3].c_str(), row[3].length());
        ret->d_street_2.castString(row[4].c_str(), row[4].length());
        ret->d_city.castString(row[5].c_str(), row[5].length());
        ret->d_state.castString(row[6].c_str(), row[6].length());
        ret->d_zip.castString(row[7].c_str(), row[7].length());
        ret->d_tax.castString(row[8].c_str(), row[8].length());
        ret->d_ytd.castString(row[9].c_str(), row[9].length());
        ret->d_next_o_id.castString(row[10].c_str(), row[10].length());
        return *ret;
    }
    // primary key (d_w_id,d_id)
};


class Customer {
public:
    Integer c_id;
    Integer c_d_id;
    Integer c_w_id;
    Varchar<16> c_first ;
    Char<2> c_middle;
    Varchar<16> c_last ;
    Varchar<20> c_street_1;
    Varchar<20> c_street_2;
    Varchar<20> c_city;
    Char<2> c_state;
    Char<9> c_zip;
    Char<16> c_phone ;
    Timestamp c_since;
    Char<2>  c_credit;
    Numeric<12,2> c_credit_lim;
    Numeric<4,4> c_discount;
    Numeric<12,2> c_balance;
    Numeric<12,2> c_ytd_paymenr;
    Numeric<4,0> c_payment_cnt ;
    Numeric<4,0> c_delivery_cnt ;
    Varchar<500> c_data;
    
    inline static Customer parse(std::vector<std::string> row) {
        Customer* ret = new Customer();
        ret->c_id.castString(row[0].c_str(), row[0].length());
        ret->c_d_id.castString(row[1].c_str(), row[1].length());
        ret->c_w_id.castString(row[2].c_str(), row[2].length());
        ret->c_first.castString(row[3].c_str(), row[3].length());
        ret->c_middle.castString(row[4].c_str(), row[4].length());
        ret->c_last.castString(row[5].c_str(), row[5].length());
        ret->c_street_1.castString(row[6].c_str(), row[6].length());
        ret->c_street_2.castString(row[7].c_str(), row[7].length());
        ret->c_city.castString(row[8].c_str(), row[8].length());
        ret->c_state.castString(row[9].c_str(), row[9].length());
        ret->c_zip.castString(row[10].c_str(), row[10].length());
        ret->c_phone.castString(row[11].c_str(), row[11].length());
        ret->c_since.castString(row[12].c_str(), row[12].length());
        ret->c_credit.castString(row[13].c_str(), row[13].length());
        ret->c_credit_lim.castString(row[14].c_str(), row[14].length());
        ret->c_discount.castString(row[15].c_str(), row[15].length());
        ret->c_balance.castString(row[16].c_str(), row[16].length());
        ret->c_ytd_paymenr.castString(row[17].c_str(), row[17].length());
        ret->c_payment_cnt.castString(row[18].c_str(), row[18].length());
        ret->c_delivery_cnt.castString(row[19].c_str(), row[19].length());
        ret->c_data.castString(row[20].c_str(), row[20].length());
        return *ret;
    }
    //primary key (c_w_id,c_d_id,c_id)
};


class History {
public:
    Integer h_c_id;
    Integer h_c_d_id;
    Integer h_c_w_id;
    Integer h_d_id;
    Integer h_w_id;
    Timestamp h_date ;
    Numeric<6,2> h_amount;
    Varchar<24> h_data ;
    
    inline static History parse(std::vector<std::string> row) {
        History* ret = new History();
        ret->h_c_id.castString(row[0].c_str(), row[0].length());
        ret->h_c_d_id.castString(row[1].c_str(), row[1].length());
        ret->h_c_w_id.castString(row[2].c_str(), row[2].length());
        ret->h_d_id.castString(row[3].c_str(), row[3].length());
        ret->h_w_id.castString(row[4].c_str(), row[4].length());
        ret->h_date.castString(row[5].c_str(), row[5].length());
        ret->h_amount.castString(row[6].c_str(), row[6].length());
        ret->h_data.castString(row[7].c_str(), row[7].length());
        return *ret;
    }
};

class NewOrder {
public:
    Integer no_o_id;
    Integer no_d_id;
    Integer no_w_id;
    
    inline static NewOrder parse(std::vector<std::string> row) {
        NewOrder* ret = new NewOrder();
        ret->no_o_id.castString(row[0].c_str(), row[0].length());
        ret->no_d_id.castString(row[1].c_str(), row[1].length());
        ret->no_w_id.castString(row[2].c_str(), row[2].length());
        return *ret;
    }
    
    //primary key (no_w_id,no_d_id,no_o_id)
};

class Order {
public:
    Integer o_id;
    Integer o_d_id;
    Integer o_w_id;
    Integer o_c_id;
    Timestamp o_entry_d ;
    Integer o_carrier_id;
    Numeric<2,0> o_ol_cnt;
    Numeric<1,0> o_all_local;
    
    inline static Order parse(std::vector<std::string> row) {
        Order* ret = new Order();
        ret->o_id.castString(row[0].c_str(), row[0].length());
        ret->o_d_id.castString(row[1].c_str(), row[1].length());
        ret->o_w_id.castString(row[2].c_str(), row[2].length());
        ret->o_c_id.castString(row[3].c_str(), row[3].length());
        ret->o_entry_d.castString(row[4].c_str(), row[4].length());
        ret->o_carrier_id.castString(row[5].c_str(), row[5].length());
        ret->o_ol_cnt.castString(row[6].c_str(), row[6].length());
        ret->o_all_local.castString(row[7].c_str(), row[7].length());
        return *ret;
    }
    // primary key (o_w_id,o_d_id,o_id)
};

class OrderLine {
public:
    Integer ol_o_id;
    Integer ol_d_id;
    Integer ol_w_id;
    Integer ol_number;
    Integer ol_i_id;
    Integer ol_supply_w_id;
    Timestamp ol_delivery_d ;
    Numeric<2,0> ol_quantity;
    Numeric<6,2> ol_amount;
    Char<64> ol_dist_info;

    inline static OrderLine parse(std::vector<std::string> row) {
        OrderLine* ret = new OrderLine();
        ret->ol_o_id.castString(row[0].c_str(), row[0].length());
        ret->ol_d_id.castString(row[1].c_str(), row[1].length());
        ret->ol_w_id.castString(row[2].c_str(), row[2].length());
        ret->ol_number.castString(row[3].c_str(), row[3].length());
        ret->ol_i_id.castString(row[4].c_str(), row[4].length());
        ret->ol_supply_w_id.castString(row[5].c_str(), row[5].length());
        ret->ol_delivery_d.castString(row[6].c_str(), row[6].length());
        ret->ol_amount.castString(row[7].c_str(), row[7].length());
        ret->ol_dist_info.castString(row[8].c_str(), row[8].length());
        return *ret;
    }
    //primary key (ol_w_id,ol_d_id,ol_o_id,ol_number)
};

class Item {
public:
    Integer i_id;
    Integer i_im_id;
    Varchar<24> i_name;
    Numeric<5,2> i_price;
    Varchar<50> i_data;
    
    inline static Item parse(std::vector<std::string> row) {
        Item* ret = new Item();
        ret->i_id.castString(row[0].c_str(), row[0].length());
        ret->i_im_id.castString(row[1].c_str(), row[1].length());
        ret->i_name.castString(row[2].c_str(), row[2].length());
        ret->i_price.castString(row[3].c_str(), row[3].length());
        ret->i_data.castString(row[4].c_str(), row[4].length());
        return *ret;
    }
    //primary key (i_id)
};

class Stock {
public:
    Integer s_i_id;
    Integer s_w_id;
    Numeric<4,0> s_quantity ;
    Char<24> s_dist_01 ;
    Char<24> s_dist_02 ;
    Char<24> s_dist_03 ;
    Char<24> s_dist_04 ;
    Char<24>  s_dist_05 ;
    Char<24> s_dist_06 ;
    Char<24>  s_dist_07 ;
    Char<24>  s_dist_08 ;
    Char<24>  s_dist_09 ;
    Char<24>  s_dist_10 ;
    Numeric<8,0> s_ytd;
    Numeric<4,0> s_order_cnt ;
    Numeric<4,0> s_remote_cnt ;
    Varchar<50> s_data;
    
    inline static Stock parse(std::vector<std::string> row) {
        Stock* ret = new Stock();
        ret->s_i_id.castString(row[0].c_str(), row[0].length());
        ret->s_w_id.castString(row[1].c_str(), row[1].length());
        ret->s_quantity.castString(row[2].c_str(), row[2].length());
        ret->s_dist_01.castString(row[3].c_str(), row[3].length());
        ret->s_dist_02.castString(row[4].c_str(), row[4].length());
        ret->s_dist_03.castString(row[5].c_str(), row[5].length());
        ret->s_dist_04.castString(row[6].c_str(), row[6].length());
        ret->s_dist_05.castString(row[7].c_str(), row[7].length());
        ret->s_dist_06.castString(row[8].c_str(), row[8].length());
        ret->s_dist_07.castString(row[9].c_str(), row[9].length());
        ret->s_dist_08.castString(row[10].c_str(), row[10].length());
        ret->s_dist_09.castString(row[11].c_str(), row[11].length());
        ret->s_dist_10.castString(row[12].c_str(), row[12].length());
        ret->s_ytd.castString(row[13].c_str(), row[13].length());
        ret->s_order_cnt.castString(row[14].c_str(), row[14].length());
        ret->s_remote_cnt.castString(row[15].c_str(), row[15].length());
        ret->s_data.castString(row[16].c_str(), row[16].length());
        return *ret;
    }
    // primary key (s_w_id,s_i_id)
};


#endif
