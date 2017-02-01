#ifndef SQLParser_HPP
#define SQLParser_HPP

#include <unordered_map>
#include <string>
#include <exception>
#include <vector>
#include <utility>
#include "SQLLexer.hpp"
#include "../parser/Schema.hpp"
#include "../parser/Query.h"

class SQLLexer;

class SQLParser {
private:
    SQLLexer& lexer;

    void parseSelect(Query& query);

    void parseFrom(Query& query);

    void parseWhere(Query& query);

public:
    class ParserException : public std::runtime_error {
    public:
        ParserException(const std::string& msg);
    };

    explicit SQLParser(SQLLexer& lexer);

    ~SQLParser();

    unique_ptr<Query> parse(Schema* schema);
};

#endif