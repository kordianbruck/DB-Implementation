
#include <iostream>
#include <dirent.h>
#include "utils/DatabaseTools.h"
//#include "tmp/db.cpp"
#include "tmp/query_b1bf7a48b4223134361205e8b4e74744.cpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "./script/schema_sys_time.sql";
    }

    //Output a nice header
    cout << "TPC-H DB Debug" << endl;
    cout << "--------------------------" << endl;

    //Load our schema, parse it and get it locally so we can parse queries on that schmea
    SchemaParser p(argv[1]);
    Schema* schema = p.parse();
    if (schema == nullptr) {
        return 1;
    }

    Database* db = make_database("./tblTemporal/");
    if (db == nullptr) {
        cout << "aborting due to schema failed to load" << endl;
        return 1;
    }

    //string date = "2017-02-01 12:00:00";
    //cout << Timestamp::castString(date.c_str(), date.length()).value << endl;
    vector<string> params;
    params.push_back("test");
    params.push_back("abc");
    query(db, params);

    //Turn off warnings caused by generated Database code, that is not available at compile time
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
    delete schema;
    delete db;
#pragma GCC diagnostic pop

    return 0;
}
