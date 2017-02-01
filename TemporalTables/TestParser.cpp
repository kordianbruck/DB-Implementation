#include "parserNew/SQLLexer.hpp"
#include "parserNew/SQLParser.hpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    SQLLexer lexer("select abc from test");
    SQLParser parser(lexer);
    SQLParser::Result res = parser.parse();

    cout << res.relations.size() << " " << res.projections.size() << " " << res.selections.size() << " " << endl;
    return 0;
}