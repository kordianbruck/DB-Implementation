#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>
#include <tuple>
#include <map>
#include <sstream>
#include "../utils/Types.hpp"
#include "../utils/DatabaseTools.h"
#include "../utils/TupelHash.h"

struct Database {
    struct Warehouse {
        using pkType = std::tuple<Integer>;

        struct Row {
            Integer w_id;
            Varchar<10> w_name;
            Varchar<20> w_street_1;
            Varchar<20> w_street_2;
            Varchar<20> w_city;
            Char<2> w_state;
            Char<9> w_zip;
            Numeric<4, 4> w_tax;
            Numeric<12, 2> w_ytd;
            Timestamp sys_start;
            Timestamp sys_end;

            pkType key() const { return std::make_tuple(w_id); }
        };

        static Row parse(const std::vector<std::string>& row) {
            auto ret = Row{};
            ret.w_id = ret.w_id.castString(row[0].c_str(), row[0].length());
            ret.w_name = ret.w_name.castString(row[1].c_str(), row[1].length());
            ret.w_street_1 = ret.w_street_1.castString(row[2].c_str(), row[2].length());
            ret.w_street_2 = ret.w_street_2.castString(row[3].c_str(), row[3].length());
            ret.w_city = ret.w_city.castString(row[4].c_str(), row[4].length());
            ret.w_state = ret.w_state.castString(row[5].c_str(), row[5].length());
            ret.w_zip = ret.w_zip.castString(row[6].c_str(), row[6].length());
            ret.w_tax = ret.w_tax.castString(row[7].c_str(), row[7].length());
            ret.w_ytd = ret.w_ytd.castString(row[8].c_str(), row[8].length());
            ret.sys_start = ret.sys_start.castString(row[9].c_str(), row[9].length());
            ret.sys_end = ret.sys_end.castString(row[10].c_str(), row[10].length());
            return ret;
        }

        std::vector<Row> table{};
        std::multimap<pkType, u_int32_t> pkHistory{};
        std::unordered_map<pkType, u_int32_t> pk{};

        size_t size() { return table.size(); }

        Row& row(pkType k) { return table[pk[k]]; }

        Row& row(size_t i) { return table[i]; }

        void update(Row& element) {
            //Move previous element to history pk
            auto& currIdx = pk[element.key()];

            //Expire the current entry (update)
            table[currIdx].sys_end = Timestamp::now();

            //Insert the new element
            element.sys_start = Timestamp::now();
            insert(element);
        }

        void remove(size_t i) {
            auto& r = row(i);
            r.sys_end = Timestamp::now();//set the deletion time
            pk.erase(r.key()); //Remove from "current" pk
            //Entry already should be in history table
        }

        void insert(Row& element) {
            if (element.sys_start == Timestamp::null()) {
                element.sys_start = Timestamp::now();
            }
            table.push_back(element);

            int idx = table.size() - 1;
            pk[element.key()] = idx;
            pkHistory.insert(make_pair(element.key(), idx));
        }

        void buildIndex() {
            size_t size = table.size();
            pk.reserve(size);
            for (size_t i = 0; i < size; i++) {
                pk[table[i].key()] = i;
                pkHistory.insert(make_pair(table[i].key(), i));
            }
        }
    };

public:
    Database(const Database&) = delete;

    Database() { }

    Warehouse warehouse;

    void import(const std::string& path) {
        DatabaseTools::loadTableFromFile(warehouse, path + "tpcc_warehouse.tbl");

        std::cout << "\tWarehouse: " << warehouse.size() << std::endl;
    }
};

extern "C" Database* make_database(std::string filename) {
    Database* db = new Database();
    db->import(filename);
    return db;
}