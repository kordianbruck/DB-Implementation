#include "parserNew/SQLLexer.hpp"
#include "parserNew/SQLParser.hpp"
#include "utils/DatabaseTools.h"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    Schema* schema = DatabaseTools::parseAndWriteSchema("./script/schema_sys_time.sql");
    if (schema == nullptr) {
        return 1;
    }

    /*SQLLexer lexer("select * from warehouse where w_id=4");
    SQLParser parser(lexer);
    try {
        QuerySelect* q = (QuerySelect*) parser.parse(schema);

        cout << q->generateQueryCode() << endl;
        delete q;
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }*/

    SQLLexer lexer2("update warehouse set w_name = 'abc' where w_id=4");
    SQLParser parser2(lexer2);
    try {
        Query* q =  parser2.parse(schema);

        cout << "QueryStruct: \n\n" << q->generateQueryCode() << endl;
        delete q;
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }

    delete schema;
    return 0;
}