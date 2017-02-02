
#include <iostream>
#include "utils/DatabaseTools.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "./script/schema_sys_time.sql";
    }

    cout << "TPC-C Compile" << endl;
    cout << "--------------------------" << endl;

    //Load our schema, parse it and get it locally so we can parse queries on that schmea
    Schema* schema = DatabaseTools::parseAndWriteSchema(argv[1]);
    if (schema == nullptr) {
        return 1;
    }

    Database* db;
    try {
        db = DatabaseTools::loadAndRunDb("db.so");
    } catch (char const* msg) {
        cerr << "Error: " << msg << endl;
        db = nullptr;
    }
    if (db == nullptr) {
        cout << "aborting due to schema failed to load" << endl;
        return 1;
    }

    cout << "Enter a sql query, 'show schema' or 'exit' to quit: " << endl;

    string line;
    do {
        if (line == "exit") {
            break;
        }

        if (line == "1") {
            line = "Select * from warehouse";
        } else if (line == "2") {
            line = "explain Select * from warehouse";
        } else if (line == "3") {
            line = "Select * from warehouse where w_id=1";
        } else if (line == "4") {
            line = "explain Select * from warehouse where w_id=1";
        } else if (line == "5") {
            line = "update warehouse set w_name = 'abc' where w_id=4";
        } else if (line == "6") {
            line = "delete from warehouse where w_id=5";
        }

        if (line == "show schema") {
            cout << schema->toString() << endl;
        } else if (line == "show performance") {
            cout << schema->toString() << endl;
        } else if (line != "") {
            try {
                string file = DatabaseTools::parseAndWriteQuery(line, schema);
                if (DatabaseTools::compileFile(file + ".cpp", file + ".so") == 0) {
                    cerr << "\tCompiled into: " << file << ". running... " << endl;
                    DatabaseTools::loadAndRunQuery(file + ".so", db);
                } else {
                    cerr << "\tCompilation failed..." << endl;
                }
            } catch (ParserError& e) {
                cerr << e.what() << " on line " << e.where() << endl;
            }
        }

        cout << ">";
    } while (getline(cin, line));

    //parseAndWriteQuery("select w_id from warehouse;", schema);
    //parseAndWriteQuery("select c_id, c_first, c_middle, c_last from customer where c_last = 'BARBARBAR';", schema);

    //Turn off warnings caused by generated Database code, that is not available at compile time
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
    delete schema;
    delete db;
#pragma GCC diagnostic pop

    return 0;
}
