#ifndef SQLParser_HPP
#define SQLParser_HPP

#include <unordered_map>
#include <string>
#include <exception>
#include <vector>
#include <utility>
#include "SQLLexer.hpp"
#include "Schema.hpp"
#include "../query/Select.h"
#include "../query/Delete.h"
#include "../query/Insert.h"
#include "../query/Update.h"

class SQLLexer;

class SQLParser {
private:
    SQLLexer& lexer;

    void parseSelect(QuerySelect*);

    void parseFrom(QuerySelect*);
    void parseFor(QuerySelect*);

    void parseWhere(Query*);

    void parseSet(QueryUpdate*);

    void parseInsertColumns(QueryInsert*);
    void parseInsertValues(QueryInsert*);

public:
    class ParserException : public std::runtime_error {
    public:
        ParserException(const std::string&);
    };

    explicit SQLParser(SQLLexer&);

    ~SQLParser();

    Query* parse(Schema*);
};

#endif