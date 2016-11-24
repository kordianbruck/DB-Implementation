
#include <iostream>
#include "parser/SchemaParser.hpp"
#include "parser/QueryParser.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "md5.h"

using namespace std;

int compileFile(string name, string outname) {
    cout << "\trunning: " << "g++ -w -O3 -std=c++14 -g tmp/" + name + " -shared -o tmp/" + outname << endl;
    int status = system(("g++ -w -O3 -std=c++14 -g tmp/" + name + " -shared -o tmp/" + outname).c_str());

    return status;
}

void* loadAndRun(string filename, string func) {
    void* handle = dlopen(("tmp/" + filename).c_str(), RTLD_NOW);
    if (!handle) {
        cerr << "error loading .so: " << dlerror() << endl;
        return nullptr;
    }
    auto fn = reinterpret_cast<int (*)(int)>(dlsym(handle, "query"));
    if (!fn) {
        cerr << "error: " << dlerror() << endl;
        exit(1);
    }

    //TODO
    //cout << fn() << endl;

    if (dlclose(handle)) {
        cerr << "error: " << dlerror() << endl;
        exit(1);
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
        myfile.open("tmp/db.h");
        myfile << schema->generateDatabaseCode();
        myfile.close();
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }

    compileFile("db.h", "db.so");
    cout << "DB ready" << endl << endl;
    return schema;
}

string parseAndWriteQuery(const string& query, Schema* s) {
    QueryParser q;
    unique_ptr<Query> qu = q.parse(query, s);
    string filename = "query_" + md5(query);

    ofstream myfile;
    myfile.open("tmp/" + filename + ".h");
    myfile << "#include <string>" << endl;
    myfile << "#include <map>" << endl;
    myfile << "#include <iostream>" << endl;
    myfile << "#include <tuple>" << endl
           << "#include \"db.h\"" << endl
           << "#include \"../Types.hpp\"" << endl
           << "#include <algorithm>" << endl;
    myfile << "using namespace std;" << endl;
    myfile << "/* " << qu.get()->toString() << " */ " << endl;
    myfile << "void query(Database* db) {" << qu.get()->generateQueryCode() << "}";
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

    cout << "Enter a sql query: " << endl;
    cout << ">";

    string line;
    while (getline(cin, line)) {
        try {
            string file = parseAndWriteQuery(line, schema);
            if (compileFile(file + ".h", file + ".so") == 0) {
                cout << "Compiled into: " << file << endl;
            } else {
                cout << "Compilation failed..." << endl;
            }
        } catch (ParserError& e) {
            cerr << e.what() << " on line " << e.where() << endl;
        }

        //Add a new placeholder for a new command
        cout << ">";
    }

    //parseAndWriteQuery("select w_id from warehouse;", schema);
    //parseAndWriteQuery("select c_id, c_first, c_middle, c_last from customer where c_last = 'BARBARBAR';", schema);

    delete schema;
    return 0;
}
