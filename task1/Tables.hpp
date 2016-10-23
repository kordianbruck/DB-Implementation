#ifndef H_Tables
#define H_Tables

#include "Types.hpp"

class warehouse {
        public:
        Integer w_id ; // PK
        Varchar<10> w_name ;
        Varchar<20> w_street_1;
        Varchar<20> w_street_2;
        Varchar<20> w_city;
        Char<2> w_state;
        Char<9> w_zip;
        Numeric<4,4> w_tax;
        Numeric<12,2> w_ytd;

}
#endif
