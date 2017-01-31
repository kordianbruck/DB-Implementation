
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
#include "../parser/QueryParser.hpp"
#include "md5.h"

using namespace std;
struct Database;

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


    static int compileFile(string name, string outname) {
        ifstream f("tmp/" + outname);
        if (f.good() && outname != "db.so") { // Only compile if not already on disk
            return 0;
        }
        //Debug symbols: -g / Additional: -flto  -pipe
        //cout << "\trunning: " << "g++ -w -O3 -std=c++14 -fPIC -pipe -flto tmp/" + name + " -shared -o tmp/" + outname << endl;
        int status = system(("g++ -O3 -std=c++14 -fPIC -flto  -pipe tmp/" + name + " -shared -o tmp/" + outname).c_str());

        return status;
    }

    static Database* loadAndRunDb(string filename) {
        void* handle = dlopen(("tmp/" + filename).c_str(), RTLD_NOW);
        if (!handle) {
            cerr << "error loading .so: " << dlerror() << endl;
            return nullptr;
        }
        auto make_database = reinterpret_cast<Database* (*)(string)>(dlsym(handle, "make_database"));
        if (!make_database) {
            cerr << "error: " << dlerror() << endl;
            return nullptr;
        }

        Database* db = make_database("../tbl/");

        if (dlclose(handle)) {
            cerr << "error: " << dlerror() << endl;
            return nullptr;
        }

        return db;
    }


    static void loadAndRunQuery(string filename, Database* db) {
        void* handle = dlopen(("tmp/" + filename).c_str(), RTLD_NOW);
        if (!handle) {
            cerr << "error loading .so: " << dlerror() << endl;
            return;
        }
        auto query = reinterpret_cast<void (*)(Database*)>(dlsym(handle, "query"));
        if (!query) {
            cerr << "error: " << dlerror() << endl;
            return;
        }

        query(db);

        if (dlclose(handle)) {
            cerr << "error: " << dlerror() << endl;
            return;
        }
    }

    static Schema* parseAndWriteSchema(const string& schemaFile) {
        Schema* schema = nullptr;
        SchemaParser p(schemaFile);
        try {
            schema = p.parse();
            cout << "Loaded " << schema->relations.size() << " relations into our schema." << endl;

            //Write to file the database
            //cout << "Generating database code..." << endl;
            ofstream myfile;
            myfile.open("tmp/db.cpp");
            myfile << schema->generateDatabaseCode();
            myfile.close();

            //cout << "Compiling." << endl << endl;
            compileFile("db.cpp", "db.so");
            //cout << "Schema ready for loading." << endl << endl;
        } catch (ParserError& e) {
            cerr << e.what() << " on line " << e.where() << endl;
        } catch (char const* msg) {
            cerr << "Error: " << msg << endl;
            schema = nullptr; //Reset pointer so that further execution stops
        }

        return schema;
    }

    static string parseAndWriteQuery(const string& query, Schema* s) {
        string filename = "query_" + md5(query);
        ifstream f("tmp/" + filename + ".so");
        if (f.good()) { // Only compile if not already on disk
            return filename;
        }

        QueryParser q;
        unique_ptr<Query> qu = q.parse(query, s);

        ofstream myfile;
        myfile.open("tmp/" + filename + ".cpp");
        myfile << "#include <string>" << endl;
        myfile << "#include <map>" << endl;
        myfile << "#include <iostream>" << endl;
        myfile << "#include <tuple>" << endl
               << "#include \"db.cpp\"" << endl
               << "#include \"../Types.hpp\"" << endl
               << "#include <algorithm>" << endl;
        myfile << "using namespace std;" << endl;
        myfile << "/* ";
        myfile << qu.get()->toString();
        myfile << " */ " << endl;
        myfile << "extern \"C\" void query(Database* db) {";
        myfile << qu.get()->generateQueryCode() << "}";
        myfile.close();

        return filename;
    }
};

#endif //TASK5_DATABASETOOLS_H
