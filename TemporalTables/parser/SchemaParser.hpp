#ifndef H_SchemaParser_hpp
#define H_SchemaParser_hpp

#include <exception>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include "Schema.hpp"
#include "ParserError.h"


struct SchemaParser {
    std::string fileName;
    std::ifstream in;
    enum class State : unsigned {
        Init,
        Create,
        Table,
        Index,
        IndexName,
        IndexOn,
        IndexTableName,
        IndexBegin,
        IndexKey,
        IndexEnd,
        CreateTableBegin,
        CreateTableEnd,
        TableName,
        Primary,
        Key,
        KeyListBegin,
        KeyName,
        KeyListEnd,
        AttributeName,
        AttributeTypeInt,
        AttributeTypeChar,
        CharBegin,
        CharValue,
        CharEnd,
        AttributeTypeNumeric,
        NumericBegin,
        NumericValue1,
        NumericSeparator,
        NumericValue2,
        NumericEnd,
        Not,
        Null,
        Separator,
        Semicolon
    };
    State state;

    SchemaParser(const std::string &fileName) : fileName(fileName), state(State::Init) {}

    ~SchemaParser();

    Schema* parse();

private:
    void nextToken(unsigned line, const std::string &token, Schema &s);
};

#endif
