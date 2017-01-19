
#include <iostream>
#include "parser/SchemaParser.hpp"
#include "parser/QueryParser.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "md5.h"

using namespace std;

struct Database;

int compileFile(string name, string outname) {
    ifstream f("tmp/" + outname);
    if (f.good()) { // Only compile if not already on disk
        return 0;
    }
    //Debug symbols: -g / Additional: -flto  -pipe
    //cout << "\trunning: " << "g++ -w -O3 -std=c++14 -fPIC -pipe -flto tmp/" + name + " -shared -o tmp/" + outname << endl;
    int status = system(("g++ -O3 -std=c++14 -fPIC -flto  -pipe tmp/" + name + " -shared -o tmp/" + outname).c_str());

    return status;
}

Database* loadAndRunDb(string filename) {
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


void loadAndRunQuery(string filename, Database* db) {
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

Schema* parseAndWriteSchema(const string& schemaFile) {
    Schema* schema;
    SchemaParser p(schemaFile);
    try {
        schema = p.parse();
        cout << "Loaded " << schema->relations.size() << " relations into our schema." << endl;
        //cout << schema->toString() << endl;

        //Write to file the database
        cout << "Generating database code..." << endl;
        ofstream myfile;
        myfile.open("tmp/db.cpp");
        myfile << schema->generateDatabaseCode();
        myfile.close();

        //cout << "Compiling." << endl << endl;
        compileFile("db.cpp", "db.so");
        cout << "Schema ready for loading." << endl << endl;
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    } catch (char const* msg) {
        cerr << "Error: " << msg << endl;
        schema = nullptr;
    }

    return schema;
}

string parseAndWriteQuery(const string& query, Schema* s) {
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
    myfile << "/* " << qu.get()->toString() << " */ " << endl;
    myfile << "extern \"C\" void query(Database* db) {" << qu.get()->generateQueryCode() << "}";
    myfile.close();

    return filename;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "./script/schema_sys_time.sql";
    }

    cout << "TPC-C Compile" << endl;
    cout << "--------------------------" << endl;

    //Load our schema, parse it and get it localally so we can parse queries on that schmea
    Schema* schema = parseAndWriteSchema(argv[1]);
    if (schema == nullptr) {
        return 1;
    }

    Database* db;
    try {
        db = loadAndRunDb("db.so");
    } catch (char const* msg) {
        cerr << "Error: " << msg << endl;
        db = nullptr;
    }
    if (db == nullptr) {
        cout << "aborting due to schema failed to load" << endl;
        return 1;
    }
    cout << "DB should be ready." << endl << endl;

    cout << "Enter a sql query or 'exit' to quit: " << endl;
    cout << ">";

    string line;
    while (getline(cin, line)) {
        if (line == "exit") {
            break;
        }
        if (line != "") {
            try {
                string file = parseAndWriteQuery(line, schema);
                if (compileFile(file + ".cpp", file + ".so") == 0) {
                    cout << "Compiled into: " << file << ". running... " << endl;
                    loadAndRunQuery(file + ".so", db);
                } else {
                    cout << "Compilation failed..." << endl;
                }
            } catch (ParserError& e) {
                cerr << e.what() << " on line " << e.where() << endl;
            }
        }

        //Add a new placeholder for a new command
        cout << ">";
    }

    //parseAndWriteQuery("select w_id from warehouse;", schema);
    //parseAndWriteQuery("select c_id, c_first, c_middle, c_last from customer where c_last = 'BARBARBAR';", schema);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
    delete schema;
    delete db;
#pragma GCC diagnostic pop

    return 0;
}
