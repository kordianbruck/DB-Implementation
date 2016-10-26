
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <utility>
#include <ctime>
#include <unordered_map>

#include "Types.hpp"
#include "table_types.hpp"


void split(std::string str, std::string sep, std::vector<std::string>& lineChunks) {
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
struct Table{
    //friend class Table;
    using KeyType = decltype(((T*)nullptr)->getKey());
    
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
    
    size_t size(){
        return table.size();
    }
    T row(KeyType k){
        return table[primary[k]];
    }
    void insert(T* element){
        table.push_back(element);
        primary[element->getKey()] = this->table.size() - 1;
    }
    void update(T& element){
        auto i = this->primary[element.getKey()];
        table[i] = element;
    }
    void buildIndex(){
        size_t size = this->table.size();
        primary.reserve(size);
        for(size_t i = 0; i < size; i++) {
            primary[table[i].getKey()] = i;
        }
    }
private:
    std::vector<T> table{};
    std::unordered_map<KeyType, u_int32_t> primary{};
    
};

#endif // TABLE_H
