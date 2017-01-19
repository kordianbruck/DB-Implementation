
#ifndef TASK5_DATABASETOOLS_H
#define TASK5_DATABASETOOLS_H

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

struct DatabaseTools {
    static void split(const std::string& str, std::vector<std::string>& lineChunks) {
        std::stringstream in(str);
        std::string segment;
        lineChunks.clear();
        while (std::getline(in, segment, '|')) {
            lineChunks.push_back(segment);
        }
    }
    template<typename T>
    static void loadTableFromFile(T& tbl, const std::string& file) {
        std::ifstream myfile(file);
        if (!myfile.is_open()) {
            return;
        }

        std::string line;
        std::vector<std::string> lineChunks;
        while (getline(myfile, line)) {
            split(line, lineChunks);
            auto tmp = T::parse(lineChunks);
            tbl.insert(tmp);
        }
        myfile.close();
    }
};

#endif //TASK5_DATABASETOOLS_H
