
#include <iostream>
#include <dirent.h>
#include "utils/DatabaseTools.h"
#include "db_tempo.cpp"
//#include "tmp/query_910dd58f34f7307fb5634b01dfde16ea.cpp"

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

    Database* db = make_database("../tbl/");
    if (db == nullptr) {
        cout << "aborting due to schema failed to load" << endl;
        return 1;
    }

    //query(db);

    //Turn off warnings caused by generated Database code, that is not available at compile time
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
    delete schema;
    delete db;
#pragma GCC diagnostic pop

    return 0;
}
