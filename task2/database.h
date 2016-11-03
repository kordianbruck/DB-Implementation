
#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>
#include <tuple>

#include "Types.hpp"
#include "tupel_hash.h"

struct Database {
private:
    struct warehouse {
        using pkType = std::tuple<Integer>;

        struct Row {
            Integer w_id; // PK
            Varchar<10> w_name;
            Varchar<20> w_street_1;
            Varchar<20> w_street_2;
            Varchar<20> w_city;
            Char<2> w_state;
            Char<9> w_zip;
            Numeric<4, 4> w_tax;
            Numeric<12, 2> w_ytd;

            pkType key() const { return std::make_tuple(w_id); }
        };

        static Row* parse(std::vector<std::string> row) {
            Row* ret = new Row();
            ret->w_id.castString(row[0].c_str(), row[0].length());
            ret->w_name.castString(row[1].c_str(), row[1].length());
            ret->w_street_1.castString(row[2].c_str(), row[2].length());
            ret->w_street_2.castString(row[3].c_str(), row[3].length());
            ret->w_city.castString(row[4].c_str(), row[4].length());
            ret->w_state.castString(row[5].c_str(), row[5].length());
            ret->w_zip.castString(row[6].c_str(), row[6].length());
            ret->w_tax.castString(row[7].c_str(), row[7].length());
            ret->w_ytd.castString(row[8].c_str(), row[8].length());
            return ret;
        }

        std::vector<Row> table{};
        std::unordered_map<pkType, u_int32_t> pk{};

        size_t size() { return table.size(); }

        Row row(pkType k) { return table[pk[k]]; }

        void update(Row& element) { table[pk[element.key()]] = element; }

        void insert(Row* element) {
            table.push_back(*element);
            pk[element->key()] = table.size() - 1;
        }

        void buildIndex() {
            size_t size = table.size();
            pk.reserve(size);
            for (size_t i = 0; i < size; i++) {
                pk[table[i].key()] = i;
            }
        }
    };


    void split(std::string str, std::string sep, std::vector<std::string>& lineChunks) {
        char* cstr = const_cast<char*>(str.c_str());
        char* current;
        lineChunks.clear();
        current = strtok(cstr, sep.c_str());
        while (current != NULL) {
            lineChunks.push_back(current);
            current = strtok(NULL, sep.c_str());
        }
    }

    template<typename T>
    void loadTableFromFile(T& tbl, const std::string& file) {
        std::ifstream myfile(file);
        if (!myfile.is_open()) {
            return;
        }

        std::string line;
        std::vector<std::string> lineChunks;
        while (getline(myfile, line)) {
            split(line, "|", lineChunks);
            auto tmp = T::parse(lineChunks);
            tbl.table.push_back(*tmp);
        }
    }

public:
    warehouse warehouse;

    void import(const std::string& path) {
        loadTableFromFile(warehouse, path + "tpcc_warehouse.tbl");
        std::cout << "\tWarehouses: " << warehouse.size() << std::endl;
    }
};

#endif // DATABASE_H
