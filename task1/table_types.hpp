#ifndef H_Tables
#define H_Tables

#include "Types.hpp"

template <typename... T>
class TableType {
public:
    using indexType = std::tuple<T...>;
    virtual void parse(std::vector<std::string>);
    virtual indexType getKey();
};

class Warehouse : public TableType<Integer> {
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
        
    inline virtual void parse(std::vector<std::string> row) final{
        this->w_id.castString(row[0].c_str(), row[0].length());
        this->w_name.castString(row[1].c_str(), row[1].length());
        this->w_street_1.castString(row[2].c_str(), row[2].length());
        this->w_street_2.castString(row[3].c_str(), row[3].length());
        this->w_city.castString(row[4].c_str(), row[4].length());
        this->w_state.castString(row[5].c_str(), row[5].length());
        this->w_zip.castString(row[6].c_str(), row[6].length());
        this->w_tax.castString(row[7].c_str(), row[7].length());
        this->w_ytd.castString(row[8].c_str(), row[8].length());
    }
  
    inline virtual indexType getKey() final{
        return indexType{w_id};
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
    
    inline void parse(std::vector<std::string> row) {
        this->d_id.castString(row[0].c_str(), row[0].length());
        this->d_w_id.castString(row[1].c_str(), row[1].length());
        this->d_name.castString(row[2].c_str(), row[2].length());
        this->d_street_1.castString(row[3].c_str(), row[3].length());
        this->d_street_2.castString(row[4].c_str(), row[4].length());
        this->d_city.castString(row[5].c_str(), row[5].length());
        this->d_state.castString(row[6].c_str(), row[6].length());
        this->d_zip.castString(row[7].c_str(), row[7].length());
        this->d_tax.castString(row[8].c_str(), row[8].length());
        this->d_ytd.castString(row[9].c_str(), row[9].length());
        this->d_next_o_id.castString(row[10].c_str(), row[10].length());
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
    
    inline void parse(std::vector<std::string> row) {
        this->c_id.castString(row[0].c_str(), row[0].length());
        this->c_d_id.castString(row[1].c_str(), row[1].length());
        this->c_w_id.castString(row[2].c_str(), row[2].length());
        this->c_first.castString(row[3].c_str(), row[3].length());
        this->c_middle.castString(row[4].c_str(), row[4].length());
        this->c_last.castString(row[5].c_str(), row[5].length());
        this->c_street_1.castString(row[6].c_str(), row[6].length());
        this->c_street_2.castString(row[7].c_str(), row[7].length());
        this->c_city.castString(row[8].c_str(), row[8].length());
        this->c_state.castString(row[9].c_str(), row[9].length());
        this->c_zip.castString(row[10].c_str(), row[10].length());
        this->c_phone.castString(row[11].c_str(), row[11].length());
        this->c_since.castString(row[12].c_str(), row[12].length());
        this->c_credit.castString(row[13].c_str(), row[13].length());
        this->c_credit_lim.castString(row[14].c_str(), row[14].length());
        this->c_discount.castString(row[15].c_str(), row[15].length());
        this->c_balance.castString(row[16].c_str(), row[16].length());
        this->c_ytd_paymenr.castString(row[17].c_str(), row[17].length());
        this->c_payment_cnt.castString(row[18].c_str(), row[18].length());
        this->c_delivery_cnt.castString(row[19].c_str(), row[19].length());
        this->c_data.castString(row[20].c_str(), row[20].length());
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
    
    inline void parse(std::vector<std::string> row) {
        this->h_c_id.castString(row[0].c_str(), row[0].length());
        this->h_c_d_id.castString(row[1].c_str(), row[1].length());
        this->h_c_w_id.castString(row[2].c_str(), row[2].length());
        this->h_d_id.castString(row[3].c_str(), row[3].length());
        this->h_w_id.castString(row[4].c_str(), row[4].length());
        this->h_date.castString(row[5].c_str(), row[5].length());
        this->h_amount.castString(row[6].c_str(), row[6].length());
        this->h_data.castString(row[7].c_str(), row[7].length());
    }
};

class NewOrder {
public:
    Integer no_o_id;
    Integer no_d_id;
    Integer no_w_id;
    
    inline void parse(std::vector<std::string> row) {
        this->no_o_id.castString(row[0].c_str(), row[0].length());
        this->no_d_id.castString(row[1].c_str(), row[1].length());
        this->no_w_id.castString(row[2].c_str(), row[2].length());
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
    
    inline void parse(std::vector<std::string> row) {
        this->o_id.castString(row[0].c_str(), row[0].length());
        this->o_d_id.castString(row[1].c_str(), row[1].length());
        this->o_w_id.castString(row[2].c_str(), row[2].length());
        this->o_c_id.castString(row[3].c_str(), row[3].length());
        this->o_entry_d.castString(row[4].c_str(), row[4].length());
        this->o_carrier_id.castString(row[5].c_str(), row[5].length());
        this->o_ol_cnt.castString(row[6].c_str(), row[6].length());
        this->o_all_local.castString(row[7].c_str(), row[7].length());
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

    inline void parse(std::vector<std::string> row) {
        this->ol_o_id.castString(row[0].c_str(), row[0].length());
        this->ol_d_id.castString(row[1].c_str(), row[1].length());
        this->ol_w_id.castString(row[2].c_str(), row[2].length());
        this->ol_number.castString(row[3].c_str(), row[3].length());
        this->ol_i_id.castString(row[4].c_str(), row[4].length());
        this->ol_supply_w_id.castString(row[5].c_str(), row[5].length());
        this->ol_delivery_d.castString(row[6].c_str(), row[6].length());
        this->ol_amount.castString(row[7].c_str(), row[7].length());
        this->ol_dist_info.castString(row[8].c_str(), row[8].length());
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
    
    inline void parse(std::vector<std::string> row) {
        this->i_id.castString(row[0].c_str(), row[0].length());
        this->i_im_id.castString(row[1].c_str(), row[1].length());
        this->i_name.castString(row[2].c_str(), row[2].length());
        this->i_price.castString(row[3].c_str(), row[3].length());
        this->i_data.castString(row[4].c_str(), row[4].length());
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
    
    inline void parse(std::vector<std::string> row) {
        this->s_i_id.castString(row[0].c_str(), row[0].length());
        this->s_w_id.castString(row[1].c_str(), row[1].length());
        this->s_quantity.castString(row[2].c_str(), row[2].length());
        this->s_dist_01.castString(row[3].c_str(), row[3].length());
        this->s_dist_02.castString(row[4].c_str(), row[4].length());
        this->s_dist_03.castString(row[5].c_str(), row[5].length());
        this->s_dist_04.castString(row[6].c_str(), row[6].length());
        this->s_dist_05.castString(row[7].c_str(), row[7].length());
        this->s_dist_06.castString(row[8].c_str(), row[8].length());
        this->s_dist_07.castString(row[9].c_str(), row[9].length());
        this->s_dist_08.castString(row[10].c_str(), row[10].length());
        this->s_dist_09.castString(row[11].c_str(), row[11].length());
        this->s_dist_10.castString(row[12].c_str(), row[12].length());
        this->s_ytd.castString(row[13].c_str(), row[13].length());
        this->s_order_cnt.castString(row[14].c_str(), row[14].length());
        this->s_remote_cnt.castString(row[15].c_str(), row[15].length());
        this->s_data.castString(row[16].c_str(), row[16].length());
    }
    // primary key (s_w_id,s_i_id)
};


#endif
