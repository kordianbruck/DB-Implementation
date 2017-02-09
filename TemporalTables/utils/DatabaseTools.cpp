
#include "DatabaseTools.h"


const string DatabaseTools::dbName = "db";
const string DatabaseTools::folderTmp = "tmp/";
const string DatabaseTools::folderTable = "./tblTemporal/";
//Debug symbols: -g  -O0 -DDEBUG -ggdb3 / Additional: -flto  -pipe
const char* DatabaseTools::cmdBuild{"g++ -O3 -std=c++14 -fPIC -flto -pipe %s -shared -o %s\0"};


void DatabaseTools::split(const std::string& str, std::vector<std::string>& lineChunks) {
    std::stringstream in(str);
    std::string segment;
    lineChunks.clear();
    while (std::getline(in, segment, '|')) {
        lineChunks.push_back(segment);
    }
}

long DatabaseTools::compileFile(const string name) {
    using namespace std::chrono;

    //Start the clock
    high_resolution_clock::time_point start = high_resolution_clock::now();

    //Assemble file names
    string fileIn = folderTmp + name + ".cpp";
    string fileOut = folderTmp + name + ".so";

    //Cache queries on disk
    ifstream f(fileOut);
    if (f.good() && name != dbName) { // Only compile if not already on disk
        return duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
    }

    //Build the command by replacing the anchors / placeholders with the correct values
    char* command = new char[strlen(cmdBuild) + fileIn.size() + fileOut.size() + 5];
    sprintf(command, cmdBuild, fileIn.c_str(), fileOut.c_str());
    int ret = system(command);
    delete[] command;

    //If compilation failed
    if (ret != 0) {
        return -1;
    }

    //Stop an return the execution time
    return duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
}

Database* DatabaseTools::loadAndRunDb(string filename) {
    void* handle = dlopen((folderTmp + filename).c_str(), RTLD_NOW);
    if (!handle) {
        cerr << "error loading .so: " << dlerror() << endl;
        return nullptr;
    }

    auto make_database = reinterpret_cast<Database* (*)(string)>(dlsym(handle, "make_database"));
    if (!make_database) {
        cerr << "error: " << dlerror() << endl;
        return nullptr;
    }
    Database* db = make_database(folderTable);

    if (dlclose(handle)) {
        cerr << "error: " << dlerror() << endl;
        return nullptr;
    }

    return db;
}


long DatabaseTools::loadAndRunQuery(string filename, Database* db, vector<string>& tmp) {
    using namespace std::chrono;

    string filenameExt = folderTmp + filename + ".so";

    //Start the clock
    high_resolution_clock::time_point start = high_resolution_clock::now();

    //Load up the dynamic lib
    void* handle = dlopen(filenameExt.c_str(), RTLD_NOW);
    if (!handle) {
        cerr << "error loading " << filenameExt << ": " << dlerror() << endl;
        return 0;
    }

    //Get the function pointer
    auto query = reinterpret_cast<void (*)(Database*, vector<string>)>(dlsym(handle, "query"));
    if (!query) {
        cerr << "error: " << dlerror() << endl;
        return 0;
    }

    //Execute
    query(db, tmp);

    //Unload
    if (dlclose(handle)) {
        cerr << "error: " << dlerror() << endl;
        return 0;
    }

    //Stop an return the execution time
    return duration_cast<microseconds>(high_resolution_clock::now() - start).count();
}

Schema* DatabaseTools::parseAndWriteSchema(const string& schemaFile) {
    Schema* schema = nullptr;
    SchemaParser p(schemaFile);
    try {
        schema = p.parse();
        cout << "Loaded " << schema->relations.size() << " relations into our schema." << endl;

        //Write to file the database
        ofstream myfile;
        myfile.open(folderTmp + dbName + ".cpp");
        myfile << schema->generateDatabaseCode();
        myfile.close();

        compileFile(dbName);
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    } catch (char const* msg) {
        cerr << "Error: " << msg << endl;
        schema = nullptr; //Reset pointer so that further execution stops
    }

    return schema;
}

string DatabaseTools::parseAndWriteQuery(const string& query, Schema* s) {
    string filename = "query_" + md5(query);
    ifstream f(folderTmp + filename + ".so");
    if (f.good()) { // Only compile if not already on disk
        return filename;
    }

    SQLLexer lexer(query);
    SQLParser q(lexer);
    Query* qu = q.parse(s);

    ofstream myfile;
    myfile.open(folderTmp + filename + ".cpp");
    myfile << "#include <string>" << endl;
    myfile << "#include <map>" << endl;
    myfile << "#include <iostream>" << endl;
    myfile << "#include <tuple>" << endl
           << "#include \"db.cpp\"" << endl
           << "#include \"../utils/Types.hpp\"" << endl
           << "#include <algorithm>" << endl
           << "#include <iomanip>" << endl;
    myfile << "using namespace std;" << endl;
    myfile << "/* ";
    myfile << qu->toString();
    myfile << " */ " << endl;
    myfile << "extern \"C\" void query(Database* db, const vector<string>& params) {" << endl;
    if (qu->shouldExplain()) {
        //Replace special characters
        string code = qu->generateQueryCode();
        boost::replace_all(code, "\"", "\\\"");
        boost::replace_all(code, "\n", "\\n");

        myfile << "cout << \"";
        myfile << code << "\";" << endl << endl;
    } else {
        myfile << qu->generateQueryCode();
    }
    myfile << "}";
    myfile.close();

    return filename;
}

void DatabaseTools::performanceTest(Schema* s, Database* db) {
    cout << "Testing performance..." << endl;
    long timeExecute;

    //Load all queries
    string queries[2];
    queries[0] = DatabaseTools::parseAndWriteQuery("INSERT INTO warehouse (w_id, w_city) VALUES (?,?)", s);
    DatabaseTools::compileFile(queries[0] );

    queries[1] = DatabaseTools::parseAndWriteQuery("UPDATE warehouse SET w_city=? WHERE w_id=?", s);
    DatabaseTools::compileFile(queries[1]);

    vector<string> params1{"15", "test"};
    timeExecute = DatabaseTools::loadAndRunQuery(queries[0], db, params1);


    vector<string> params2;
    params2.push_back("test_update");
    params2.push_back("15");
    timeExecute = DatabaseTools::loadAndRunQuery(queries[1], db, params2);

    //TODO
    //Manually do a compiled insert + update vs one update
}
