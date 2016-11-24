
#include <iostream>
#include "parser/SchemaParser.hpp"
#include "parser/QueryParser.hpp"
#include "operators/TableScan.h"
#include "operators/HashJoin.h"
#include "operators/Selection.h"
#include "operators/Print.h"
#include "md5.h"

using namespace std;

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
        myfile.open("tmp/db.h");
        myfile << schema->generateDatabaseCode();
        myfile.close();
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }
    cout << "All done" << endl << endl << endl;

    //compile it

    return schema;
}

string parseAndWriteQuery(const string& query, Schema* s) {
    QueryParser q;
    unique_ptr<Query> qu = q.parse(query, s);
    string filename = "tmp/query_" + md5(query) + ".h";

    ofstream myfile;
    myfile.open(filename);
    myfile << "#include <string>" << endl;
    myfile << "#include <map>" << endl;
    myfile << "#include <iostream>" << endl;
    myfile << "#include <tuple>\n"
            "#include \"../Types.hpp\"\n"
            "#include <algorithm>" << endl;
    myfile << "using namespace std;" << endl;
    try {
        myfile << "/* " << qu.get()->toString() << " */ " << endl;
        myfile << "void query(Database* db) {" << qu.get()->generateQueryCode() << "}";
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }
    myfile.close();

    return filename;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        argv[1] = (char*) "script/schema.sql";
    }

    cout << "TPC-C Compile" << endl;
    cout << "--------------------------" << endl;

    //Load our schema, parse it and get it localally so we can parse queries on that schmea
    Schema* schema = parseAndWriteSchema(argv[1]);

    parseAndWriteQuery("select w_id from warehouse;", schema);
    //parseAndWriteQuery("select c_id, c_first, c_middle, c_last from customer where c_last = 'BARBARBAR';", schema);

    delete schema;
    return 0;
}
