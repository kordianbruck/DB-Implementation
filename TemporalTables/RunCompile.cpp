
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include "utils/DatabaseTools.h"

string testQueries[] = {
        "Select * from warehouse",
        "explain Select * from warehouse",
        "Select * from warehouse where w_id=1",
        "explain Select * from warehouse where w_id=1",
        "update warehouse set w_name = 'abc' where w_id=4",
        "delete from warehouse where w_id=5",
        "INSERT INTO warehouse (w_id) VALUES (6)"
};

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "./script/schema_sys_time.sql";
    }

    //Delete old queries
    system("exec rm -r ./tmp/*");

    //Output a nice header
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

    //Output some Info & Enable input
    cout << "Enter a sql query, 'show queries', 'show schema' or 'exit' to quit: " << endl;
    string line;
    do {
        //Do we want to exit?
        if (line == "exit") {
            break;
        }

        //check if we want to run a stored query
        if (boost::starts_with(line, "run")) {
            int tmp = stoi(line.substr(4, string::npos));
            if (tmp <= testQueries->size() && tmp >= 0) {
                line = testQueries[tmp];
            } else {
                cout << "Query not found!" << endl;
            }
        }

        //A few "magic" commands
        if (line == "show queries") { //Show any stored example queries
            int i = 0;
            for (auto& e: testQueries) {
                cout << i << ": " << e << endl;
                i++;
            }
        } else if (line == "show schema") { //Display the database schema
            cout << schema->toString() << endl;
        } else if (line == "show performance") { //Performance test the database
            cout << schema->toString() << endl;
        } else if (line != "") { //Actually proccess queries
            try {
                string file = DatabaseTools::parseAndWriteQuery(line, schema);
                if (DatabaseTools::compileFile(file + ".cpp", file + ".so") == 0) {
                    //cerr << "\tCompiled into: " << file << ". running... " << endl;
                    DatabaseTools::loadAndRunQuery(file + ".so", db);
                } else {
                    cerr << "\tCompilation failed..." << endl;
                }
            } catch (ParserError& e) {
                cerr << e.what() << " on line " << e.where() << endl;
            } catch (SQLParser::ParserException& e) {
                cerr << e.what() << endl;
            }
        }

        cout << ">";
    } while (getline(cin, line));

    //Turn off warnings caused by generated Database code, that is not available at compile time
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
    delete schema;
    delete db;
#pragma GCC diagnostic pop

    return 0;
}
