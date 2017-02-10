#include <iomanip>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include "utils/DatabaseTools.h"

string testQueries[] = {
        "SELECT * FROM warehouse", //0
        "SELECT * FROM warehouse WHERE w_id=1", //1
        "SELECT * FROM warehouse FOR SYSTEM_TIME AS OF '2017-02-05 12:00:00'", //2
        "EXPLAIN SELECT * FROM warehouse WHERE w_id=1",//3
        "UPDATE warehouse SET w_name = 'abc' WHERE w_id=4",//4
        "DELETE FROM warehouse WHERE w_id=4",//5
        "INSERT INTO warehouse (w_id) VALUES (6)",//6
        "SELECT * FROM warehouse FOR SYSTEM_TIME AS OF '2017-02-08 11:22:42' WHERE w_id=1",//7
};

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "./script/schema_performance.sql";
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
    cout << "Enter a sql query, 'show queries', 'run <query>', 'show performance', 'show schema' or 'exit' to quit: " << endl;
    string line;
    long timeCompile = 0, timeExecute = 0;
    //Pass an empty vector as we don't support prepared statements on the console
    vector<string> parameters;
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
            DatabaseTools::performanceTest(schema, db);
        } else if (line != "") { //Actually proccess queries
            try {
                string file = DatabaseTools::parseAndWriteQuery(line, schema);
                timeCompile = DatabaseTools::compileFile(file);
                if (timeCompile >= 0) {
                    timeExecute = DatabaseTools::loadAndRunQuery(file, db, parameters);
                } else {
                    cerr << "\tCompilation failed..." << endl;
                }
                cout << "\033[34mCompile: " << timeCompile << "ms / Execute: " << timeExecute << "us \033[0m" << endl;
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
