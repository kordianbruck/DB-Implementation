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

    SQLLexer lexer("select * from warehouse where w_id=4");
    SQLParser parser(lexer);
    try {
        unique_ptr<Query> q = parser.parse(schema);

        cout << q.get()->generateQueryCode() << endl;
    } catch (ParserError& e) {
        cerr << e.what() << " on line " << e.where() << endl;
    }

    delete schema;
    return 0;
}