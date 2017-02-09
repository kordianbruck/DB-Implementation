
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
#include <string>
#include <stdlib.h>
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include "../parser/SchemaParser.hpp"
#include "../parser/SQLLexer.hpp"
#include "../parser/SQLParser.hpp"
#include "md5.h"
#include <boost/algorithm/string/replace.hpp>

using namespace std;
struct Database;

struct DatabaseTools {

    static const string dbName;
    static const string dbNameCompiled;
    static const string folderTmp;
    static const string folderTable;
    static const char* cmdBuild;

    static void split(const std::string& str, std::vector<std::string>& lineChunks);

    static long compileFile(const string& name, const string& outname);

    static Database* loadAndRunDb(string filename);

    static long loadAndRunQuery(string filename, Database* db);

    static Schema* parseAndWriteSchema(const string& schemaFile);

    static string parseAndWriteQuery(const string& query, Schema* s);

    static void performanceTest( Schema* s);

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
