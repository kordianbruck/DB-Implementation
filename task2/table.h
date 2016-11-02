
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>
#include <tuple>

#include "Types.hpp"
#include "table_types.hpp"
#include "tupel_hash.h"

template <typename T>
class Table{
public:
    //friend class Table;
    using primaryType = decltype(((T*)nullptr)->key());
    
    inline void split(std::string str, std::string sep, std::vector<std::string>& lineChunks) {
        char* cstr=const_cast<char*>(str.c_str());
        char* current;
        lineChunks.clear();
        current=strtok(cstr, sep.c_str());
        while(current!=NULL) {
            lineChunks.push_back(current);
            current=strtok(NULL,sep.c_str());
        } 
    }
    
    inline void loadTableFromFile(const std::string& file){
        std::ifstream myfile(file);
        if (!myfile.is_open()) {
            return;
        }

        std::string line;
        std::vector<std::string> lineChunks;
        while ( getline (myfile,line) ) {
            split(line,"|", lineChunks);
            T* tmp = new T();
            tmp->parse(lineChunks);
            table.push_back(*tmp);
        }
    }
    
    inline size_t size(){
        return table.size();
    }
    
    inline T row(primaryType k){
        return table[primary[k]];
    }
    
    inline void insert(T* element){
        table.push_back(*element);
        primary[element->key()] = this->table.size() - 1;
    }
    
    inline void update(T& element){
        auto i = this->primary[element.key()];
        table[i] = element;
    }
    
    inline void buildIndex(){
        size_t size = this->table.size();
        primary.reserve(size);
        for(size_t i = 0; i < size; i++) {
            primary[table[i].key()] = i;
        }
    }
private:
    std::vector<T> table{};
    std::unordered_map<primaryType, u_int32_t> primary{};
    
};

#endif // TABLE_H
